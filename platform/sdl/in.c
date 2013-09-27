#include "in.h"

in_state_t in_state;

cvar_t	*in_joystick;

void (*RW_IN_Init_fp)(in_state_t *in_state_p) = NULL;
void (*RW_IN_Shutdown_fp)(void) = NULL;
void (*RW_IN_Activate_fp)(qboolean active) = NULL;
void (*RW_IN_Commands_fp)(void) = NULL;
void (*RW_IN_Move_fp)(usercmd_t *cmd) = NULL;
void (*RW_IN_Frame_fp)(void) = NULL;

void IN_Init (void)
{
	in_joystick	= Cvar_Get ("in_joystick", "0", CVAR_ARCHIVE);
	if (RW_IN_Init_fp)
		RW_IN_Init_fp(&in_state);
}

void IN_Shutdown (void)
{
	if (RW_IN_Shutdown_fp)
		RW_IN_Shutdown_fp();
}

void IN_Commands (void)
{
	if (RW_IN_Commands_fp)
		RW_IN_Commands_fp();
}

void IN_Move (usercmd_t *cmd)
{
	if (RW_IN_Move_fp)
		RW_IN_Move_fp(cmd);
}

void IN_Frame (void)
{
	if (RW_IN_Activate_fp) 
	{
		if ( !cl.refresh_prepped || cls.key_dest == key_console || cls.key_dest == key_menu)
			RW_IN_Activate_fp(false);
		else
			RW_IN_Activate_fp(true);
	}

	if (RW_IN_Frame_fp)
		RW_IN_Frame_fp();
}

void IN_Activate (qboolean active)
{
}
