/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <ctype.h>
#include "qcommon.h"

// QUAKE FILESYSTEM

/* This is the completely overhauled filesystem handler. It throws away many of
 * the previous features, including pak support and the ability to overlay
 * sources. Instead, we go with a more straight forward approach here.
 * 
 * All game related files are stored and searched in fs_gameDir, which equals to
 * GAMES_DIRECTORY/Cvar(game). This is also exported as "fs_gameDir" Cvar.
 * 
 * It also maintains Cvar "fs_userDir", a directory made for holding everything
 * the user may add to the game. This includes savegames, downloads,
 * screenshots, ...
 *
 * 
 * Old:
 * The "base directory" is the path to the directory holding the quake.exe and
 * all game directories.  The sys_* files pass this to host_init in
 * quakeparms_t->basedir.  This can be overridden with the "-basedir" command
 * line parm to allow code debugging in a different directory.  The base
 * directory is only used during filesystem initialization.
 * 
 * The "game directory" is the first tree on the search path and directory that
 * all generated files (savegames, screenshots, demos, config files) will be
 * saved to.  This can be overridden with the "-game" command line parameter. 
 * The game directory can never be changed while quake is executing.  This is a
 * precacution against having a malicious server instruct clients to write files
 *  over areas they shouldn't.
*/

static char *fs_gameDir = NULL; // Absolute paths
static char *fs_userDir = NULL;

static char *strtolower(const char *str) {
	char *ret = malloc(strlen(str));
	
	int i;
	for (i = 0; i < strlen(str); i++) {
		ret[i] = tolower(str[i]);
	}
	
	return ret;
}

static FILE *_FS_OpenWithBase(char *base, char *filename) {
	FILE *ret;
	
	char *pathBuf = malloc(strlen(base) + 1 + strlen(filename) + 1);
	sprintf(pathBuf, "%s/%s", base, filename);
	ret = fopen(pathBuf, "rb");
	free(pathBuf);
	
	return ret;
}

int FS_OpenFile (char *filename, FILE **file) {
	char *lower = strtolower(filename);
	
	*file = _FS_OpenWithBase(fs_userDir, lower);
	if (*file == NULL)
		*file = _FS_OpenWithBase(fs_gameDir, lower);
		
	free(lower);
	if (*file != NULL)
		return FS_FileLength(*file);
	return 0;
}

void FS_CloseFile (FILE *f) {
    fclose (f);
}

#define    MAX_READ    0x10000 // read in blocks of 64k
void FS_Read (void *buffer, int len, FILE *f) {
    int block, remaining;
    int read;
    byte *buf;
    int tries;

    buf = (byte *)buffer;

    // read in chunks for progress bar
    remaining = len;
    tries = 0;
    while (remaining)
    {
        block = remaining;
        if (block > MAX_READ)
            block = MAX_READ;
        read = fread (buf, 1, block, f);
        if (read == 0)
			Com_Error (ERR_FATAL, "FS_Read: 0 bytes read");

        if (read == -1)
            Com_Error (ERR_FATAL, "FS_Read: -1 bytes read");

        // do some progress bar thing here...

        remaining -= read;
        buf += read;
    }
}

int FS_LoadFile (char *path, void **buffer) {
    FILE *h;
    byte *buf;
    int len;

    len = FS_OpenFile (path, &h);
    if (!h) {
        if (buffer)
            *buffer = NULL;
        return -1;
    }
    
    if (!buffer) {
        FS_CloseFile(h);
        return len;
    }

    buf = Z_Malloc(len);
    *buffer = buf;

    FS_Read (buf, len, h);

    fclose (h);
	return len;
}

void FS_FreeFile (void *buffer) {
    Z_Free (buffer);
}


int FS_FileLength (FILE *f) {
    int pos = ftell(f);
    fseek (f, 0, SEEK_END);
    int end = ftell(f);
    fseek (f, pos, SEEK_SET);

    return end;
}

void FS_CreatePath(char *path) {
    char *ofs;
    
    for (ofs = path+1 ; *ofs ; ofs++) {
        if (*ofs == '/') {    // create the directory
            *ofs = 0;
            Sys_Mkdir (path);
            *ofs = '/';
        }
    }
}

char **FS_ListFiles( char *findname, int *numfiles, unsigned musthave, unsigned canthave) {
    char *s;
    int nfiles = 0;
    char **list = 0;

    s = Sys_FindFirst( findname, musthave, canthave );
    while ( s )
    {
        if ( s[strlen(s)-1] != '.' )
            nfiles++;
        s = Sys_FindNext( musthave, canthave );
    }
    Sys_FindClose ();

    if ( !nfiles )
        return NULL;

    nfiles++; // add space for a guard
    *numfiles = nfiles;

    list = malloc( sizeof( char * ) * nfiles );
    memset( list, 0, sizeof( char * ) * nfiles );

    s = Sys_FindFirst( findname, musthave, canthave );
    nfiles = 0;
    while ( s )
    {
        if ( s[strlen(s)-1] != '.' )
        {
            list[nfiles] = strdup( s );
#ifdef _WIN32
            strlwr( list[nfiles] );
#endif
            nfiles++;
        }
        s = Sys_FindNext( musthave, canthave );
    }
    Sys_FindClose ();

    return list;
}

char *FS_GameDir() {
	return fs_gameDir;
}

char *FS_UserDir() {
	return fs_userDir;
}

void FS_ExecAutoexec() {
#if 0
    char *dir;
    char name [MAX_QPATH];

    dir = Cvar_VariableString("gamedir");
    if (*dir)
        Com_sprintf(name, sizeof(name), "%s/%s/autoexec.cfg", fs_basedir->string, dir); 
    else
        Com_sprintf(name, sizeof(name), "%s/%s/autoexec.cfg", fs_basedir->string, BASEDIRNAME); 
    if (Sys_FindFirst(name, 0, SFF_SUBDIR | SFF_HIDDEN | SFF_SYSTEM))
        Cbuf_AddText ("exec autoexec.cfg\n");
    Sys_FindClose();
#endif
}

static void _FS_SetDefaults() {
	FS_SetGame(DEFAULT_GAME);
	
	/* This shouldn't be necessary in theory, but other code seems to depend on that.
	   Just do it for now... */
	Cvar_FullSet("game", DEFAULT_GAME, CVAR_SERVERINFO | CVAR_NOSET);
}

void FS_SetGame(char *dir) {
	if (dir[0] == '\0') {
		_FS_SetDefaults();
		return;
	}
	
    if (strstr(dir, "..") || strstr(dir, "/")
        || strstr(dir, "\\") || strstr(dir, ":") )
    {
        Com_Printf ("Gamedir should be a single filename, not a path\n");
        return;
    }

    // flush all data, so it will be forced to reload
    if (dedicated && !dedicated->value)
        Cbuf_AddText ("vid_restart\nsnd_restart\n");

	if (fs_gameDir != NULL)
		free(fs_gameDir);
	fs_gameDir = malloc(strlen(GAMES_DIRECTORY) + 1 + strlen(dir) + 1);
	sprintf(fs_gameDir, "%s/%s", GAMES_DIRECTORY, dir);
	
	Cvar_FullSet("fs_gameDir", fs_gameDir, CVAR_SERVERINFO | CVAR_NOSET);
}

void FS_InitFilesystem() {
    // Load game
    cvar_t *game = Cvar_Get("game", DEFAULT_GAME, CVAR_LATCH|CVAR_SERVERINFO);
    FS_SetGame(game->string);
    
    fs_userDir = "/home/rika/q2test";
}
