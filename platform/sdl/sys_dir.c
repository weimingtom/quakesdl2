#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// TODO UNIFY remove
#include "../game/q_shared.h"

/* Directory functions */
/* These are implemented without SDL, but every target platform implements 
 * this, so whatever... 
 * Taken straight from platform/linux/q_shlinux.c */

static char findbase[MAX_OSPATH];
static char findpath[MAX_OSPATH];
static char findpattern[MAX_OSPATH];
static DIR *fdir;

static qboolean CompareAttributes(char *path, char *name,
	unsigned musthave, unsigned canthave )
{
	struct stat st;
	char fn[MAX_OSPATH];

	// . and .. never match
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
		return false;

	return true;

	if (stat(fn, &st) == -1)
		return false; // shouldn't happen

	if ( ( st.st_mode & S_IFDIR ) && ( canthave & SFF_SUBDIR ) )
		return false;

	if ( ( musthave & SFF_SUBDIR ) && !( st.st_mode & S_IFDIR ) )
		return false;

	return true;
}

char *Sys_FindFirst (char *path, unsigned musthave, unsigned canhave) {
	struct dirent *d;
	char *p;

	if (fdir)
		Sys_Error ("Sys_BeginFind without close");

	strcpy(findbase, path);

	if ((p = strrchr(findbase, '/')) != NULL) {
		*p = 0;
		strcpy(findpattern, p + 1);
	} else
		strcpy(findpattern, "*");

	if (strcmp(findpattern, "*.*") == 0)
		strcpy(findpattern, "*");
	
	if ((fdir = opendir(findbase)) == NULL)
		return NULL;
	while ((d = readdir(fdir)) != NULL) {
		if (!*findpattern || glob_match(findpattern, d->d_name)) {
			if (CompareAttributes(findbase, d->d_name, musthave, canhave)) {
				sprintf (findpath, "%s/%s", findbase, d->d_name);
				return findpath;
			}
		}
	}
	return NULL;
}

char *Sys_FindNext (unsigned musthave, unsigned canhave) {
	struct dirent *d;

	if (fdir == NULL)
		return NULL;
	
	while ((d = readdir(fdir)) != NULL) {
		if (!*findpattern || glob_match(findpattern, d->d_name)) {
			if (CompareAttributes(findbase, d->d_name, musthave, canhave)) {
				sprintf (findpath, "%s/%s", findbase, d->d_name);
				return findpath;
			}
		}
	}
	return NULL;
}

void Sys_FindClose() {
	if (fdir != NULL)
		closedir(fdir);
	fdir = NULL;
}

void Sys_Mkdir(char *path) {
    mkdir (path, 0777);
}
