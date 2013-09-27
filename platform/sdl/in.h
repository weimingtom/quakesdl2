#ifndef IN_H
#define IN_H

#include "../client/client.h"
#include "kbd.h" // For Key_Event_fp_t

typedef struct in_state {
	// Pointers to functions back in client, set by vid_so
	void (*IN_CenterView_fp)(void);
	Key_Event_fp_t Key_Event_fp;
	vec_t *viewangles;
	int *in_strafe_state;
} in_state_t;

extern in_state_t in_state;

extern void (*RW_IN_Init_fp)(in_state_t *in_state_p);
extern void (*RW_IN_Shutdown_fp)(void);
extern void (*RW_IN_Activate_fp)(qboolean active);
extern void (*RW_IN_Commands_fp)(void);
extern void (*RW_IN_Move_fp)(usercmd_t *cmd);
extern void (*RW_IN_Frame_fp)(void);

#endif /* IN_H */
