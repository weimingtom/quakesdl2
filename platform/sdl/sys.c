#include <stdlib.h>
#include <stdarg.h>

#include <SDL.h>
#include <SDL_log.h>
#include <SDL_loadso.h>

#include <sys/time.h>

#include "../client/client.h"
#include "kbd.h"

unsigned sys_frame_time;
int sys_current_time;

static void *_sys_gameso = NULL;

/* Internal utility functions */
static void _Sys_Quit(int status) {
	CL_Shutdown();
	Qcommon_Shutdown();
	
	SDL_Quit();
	exit(status);
}

/* Log */
void Sys_Error(char *format, ...) {
	va_list ap;
	va_start(ap, format);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, format, ap);
	va_end(ap);
	
	_Sys_Quit(1);
}

void Sys_ConsoleOutput(char *string) {
	//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, string);
	printf("%s", string);
}

char *Sys_ConsoleInput() {
	return NULL;
}

/* Time */
/* These are implemented without SDL, but every target platform implements 
 * this, so whatever... 
 * Taken straight from platform/linux/q_shlinux.c */
int Sys_Milliseconds() {
	struct timeval tp;
	struct timezone tzp;
	static int secbase;

	gettimeofday(&tp, &tzp);
	
	if (!secbase) {
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	sys_current_time = (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;
	
	return sys_current_time;
}

/* Keyboard */
void Sys_SendKeyEvents() {
	if (KBD_Update_fp)
		KBD_Update_fp();

	// grab frame time 
	sys_frame_time = Sys_Milliseconds();
}

/* Clipboard */
char *Sys_GetClipboardData() {
	return NULL;
}

/* Game handling */
void *Sys_GetGameAPI(void *parms) {
	if (_sys_gameso != NULL) {
		Com_Printf("Sys_GetGameAPI: There's already a game loaded!\n");
		return NULL;
	}
	
	Com_Printf("Loading game...\n");
	
	char *pathBuf = malloc(strlen(FS_GameDir()) + 1 + strlen("libgame.so") + 1);
	strcpy(pathBuf, FS_GameDir());
	strcat(pathBuf, "/libgame.so");
	
	_sys_gameso = SDL_LoadObject(pathBuf);
	if (_sys_gameso == NULL)
		return NULL;
		
	void *(*f)(void *) = SDL_LoadFunction(_sys_gameso, "GetGameAPI");
	if (f == NULL) {
		Sys_UnloadGame();
		return NULL;
	}
	
	return (*f)(parms);
}

void Sys_UnloadGame() {
	if (_sys_gameso != NULL) {
		SDL_UnloadObject(_sys_gameso);
		_sys_gameso = NULL;
	}
}

/* Misc */
void Sys_AppActivate() {
}

char *strlwr(char *s){
	while (*s) {
		*s = tolower(*s);
		s++;
	}
}

/* Initialization and deinitalization */
void Sys_Init() {
	SDL_Init(0);
}

void Sys_Quit() {
	_Sys_Quit(0);
}

int main (int argc, char **argv)
{
	int time, oldtime, newtime;

	Qcommon_Init(argc, argv);

    oldtime = Sys_Milliseconds ();
    for (;;) {
	// find time spent rendering last frame
		do {
			newtime = Sys_Milliseconds ();
			time = newtime - oldtime;
		} while (time < 1);
        Qcommon_Frame(time);
		oldtime = newtime;
    }
}
