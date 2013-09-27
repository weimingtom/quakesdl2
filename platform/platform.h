#ifndef PLATFORM_H
#define PLATFORM_H

// TODO UNIFY Unify these
#define	MAX_OSPATH 128 // max length of a filesystem pathname

void	Sys_Init (void);
void	Sys_AppActivate (void);

void	Sys_UnloadGame (void);
void	*Sys_GetGameAPI (void *parms);

char	*Sys_ConsoleInput (void);
void	Sys_ConsoleOutput (char *string);
void	Sys_SendKeyEvents (void);
void	Sys_Error (char *error, ...);
void	Sys_Quit (void);
char	*Sys_GetClipboardData( void );

#endif /* PLATFORM_H */
