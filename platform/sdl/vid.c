/* Quake 2 SDL2 platform video implementation */

#include "config.h"
#include "kbd.h"
#include "in.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL.h>

#include "../client/client.h"

viddef_t viddef;
refexport_t re;

cvar_t *vid_ref;
cvar_t *vid_xpos;
cvar_t *vid_ypos;
cvar_t *vid_fullscreen;
cvar_t *vid_gamma;

static void *_vid_refso = NULL;

/* Glue */
#define	MAXPRINTMSG	4096
void VID_Printf (int print_level, char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static qboolean	inupdate;
	
	va_start (argptr,fmt);
	vsprintf (msg,fmt,argptr);
	va_end (argptr);

	if (print_level == PRINT_ALL)
		Com_Printf ("%s", msg);
	else
		Com_DPrintf ("%s", msg);
}

void VID_Error (int err_level, char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static qboolean	inupdate;
	
	va_start (argptr,fmt);
	vsprintf (msg,fmt,argptr);
	va_end (argptr);

	Com_Error (err_level,"%s", msg);
}

/* Modes */
typedef struct vidmode_s
{
	const char *description;
	int         width, height;
	int         mode;
} vidmode_t;

vidmode_t vid_modes[] =
{
	{ "Mode 0: 320x240",    320, 240,   0 },
	{ "Mode 1: 400x300",    400, 300,   1 },
	{ "Mode 2: 512x384",    512, 384,   2 },
	{ "Mode 3: 640x480",    640, 480,   3 },
	{ "Mode 4: 800x600",    800, 600,   4 },
	{ "Mode 5: 960x720",    960, 720,   5 },
	{ "Mode 6: 1024x768",   1024, 768,  6 },
	{ "Mode 7: 1152x864",   1152, 864,  7 },
	{ "Mode 8: 1280x1024",  1280, 1024, 8 },
	{ "Mode 9: 1600x1200",  1600, 1200, 9 },
	{ "Mode 10: 2048x1536", 2048, 1536, 10 },
	{ "Mode 11: 1366x768",  1366, 768, 11 }
};
#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[0] ) )

qboolean VID_GetModeInfo(int *width, int *height, int mode) {
	if ( mode < 0 || mode >= VID_NUM_MODES )
		return false;

	*width  = vid_modes[mode].width;
	*height = vid_modes[mode].height;

	return true;
}

void VID_NewWindow (int width, int height) {
	viddef.width  = width;
	viddef.height = height;
}

/* Console command to re-start the video mode and refresh DLL. We do this
 * simply by setting the modified flag for the vid_ref variable, which will
 * cause the entire video mode and refresh DLL to be reset on the next frame. */
void VID_Restart_f() {
	vid_ref->modified = true;
}

void VID_Init() {
	vid_ref = Cvar_Get ("vid_ref", DEFAULT_REFRESH_ENGINE, CVAR_ARCHIVE);
	vid_xpos = Cvar_Get ("vid_xpos", "3", CVAR_ARCHIVE);
	vid_ypos = Cvar_Get ("vid_ypos", "22", CVAR_ARCHIVE);
	vid_fullscreen = Cvar_Get ("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get( "vid_gamma", "1", CVAR_ARCHIVE );
	
	Cmd_AddCommand ("vid_restart", VID_Restart_f);
}

void VID_Shutdown() {
}

static char *_VID_RefName2So(const char *name) {
	static char buf[512];
	
	strcpy(buf, "libref_");
	strcat(buf, name);
	strcat(buf, ".so");
	
	return buf;
}

void VID_FreeRefresh() {
	SDL_UnloadObject(_vid_refso);
	
	// Null everything
	_vid_refso = NULL;
	KBD_Init_fp = NULL;
	KBD_Update_fp = NULL;
	KBD_Close_fp = NULL;
}

void VID_UnloadRefresh() {
	if (_vid_refso == NULL)
		return;
	// Shutdown stuff
	KBD_Close();
	// close IN
	re.Shutdown();
	
	// Unload stuff
	VID_FreeRefresh();
}

void Do_Key_Event(int key, qboolean down) {
	Key_Event(key, down, Sys_Milliseconds());
}

qboolean VID_LoadRefresh(char *name) {
	refimport_t	ri;
	GetRefAPI_t	GetRefAPI;
	char	fn[MAX_OSPATH];
	struct stat st;
	FILE *fp;
	
	VID_UnloadRefresh();

	Com_Printf("------- Loading %s -------\n", name);

	strcpy(fn, REFRESH_ENGINE_DIRECTORY);
	strcat(fn, "/");
	strcat(fn, name);

	_vid_refso = SDL_LoadObject(fn);
	if (_vid_refso == NULL) {
		Com_Printf("Can't load %s from %s: %s\n", name, fn, SDL_GetError());
		return false;
	}
	GetRefAPI = SDL_LoadFunction(_vid_refso, "GetRefAPI");
	if (GetRefAPI == NULL) {
		Com_Error(ERR_FATAL, "Can't load %s: GetRefAPI not found", name);
		return false;
	}

	// Fill in imports for ref
	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Con_Printf = VID_Printf;
	ri.Sys_Error = VID_Error;
	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_UserDir = FS_UserDir;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Vid_GetModeInfo = VID_GetModeInfo;
	ri.Vid_MenuInit = VID_MenuInit;
	ri.Vid_NewWindow = VID_NewWindow;

	// Hand them over and get exports
	re = GetRefAPI(ri);
	
	if (re.api_version != API_VERSION) {
		VID_FreeRefresh();
		Com_Error(ERR_FATAL, "%s has incompatible api_version", name);
	}

	/* Init KBD */
	if ((KBD_Init_fp = SDL_LoadFunction(_vid_refso, "KBD_Init")) == NULL ||
		(KBD_Update_fp = SDL_LoadFunction(_vid_refso, "KBD_Update")) == NULL ||
		(KBD_Close_fp = SDL_LoadFunction(_vid_refso, "KBD_Close")) == NULL)
		Sys_Error("No KBD functions in REF.\n");

	KBD_Init_fp(Do_Key_Event);
	
	/* Init IN */
	in_state.IN_CenterView_fp = IN_CenterView;
	in_state.Key_Event_fp = Do_Key_Event;
	in_state.viewangles = cl.viewangles;
	in_state.in_strafe_state = &in_strafe.state;

	if ((RW_IN_Init_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Init")) == NULL ||
		(RW_IN_Shutdown_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Shutdown")) == NULL ||
		(RW_IN_Activate_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Activate")) == NULL ||
		(RW_IN_Commands_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Commands")) == NULL ||
		(RW_IN_Move_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Move")) == NULL ||
		(RW_IN_Frame_fp = SDL_LoadFunction(_vid_refso, "RW_IN_Frame")) == NULL)
		Sys_Error("No RW_IN functions in REF.\n");

	IN_Init();

	if (re.Init( 0, 0 ) == -1) {
		re.Shutdown();
		VID_UnloadRefresh();
		return false;
	}

	Com_Printf( "------------------------------------\n");
	return true;
}

/* This function gets called once just before drawing each frame, and it's sole
 * purpose in life is to check to see if any of the video mode parameters have
 * changed, and if they have to update the rendering DLL and/or video mode to
 * match. */
void VID_CheckChanges() {
	char *name;

	if (vid_ref->modified)
		S_StopAllSounds();

	while (vid_ref->modified) {
		// refresh has changed
		vid_ref->modified = false;
		vid_fullscreen->modified = true;
		cl.refresh_prepped = false;
		cls.disable_screen = true;

		name = _VID_RefName2So(vid_ref->string);
		if (!VID_LoadRefresh(name)) {
			Com_Error(ERR_FATAL, "Couldn't load refresh!");
			// drop the console if we fail to load a refresh
			if (cls.key_dest != key_console)
				Con_ToggleConsole_f();
		}
		cls.disable_screen = false;
	}
}
