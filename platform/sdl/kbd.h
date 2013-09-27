#ifndef KBD_H
#define KBD_H

#include "../client/client.h"

typedef void (*Key_Event_fp_t)(int key, qboolean down);

extern void (*KBD_Init_fp)(Key_Event_fp_t fp);
extern void (*KBD_Update_fp)(void);
extern void (*KBD_Close_fp)(void);

void KBD_Init(Key_Event_fp_t fp);
void KBD_Update();
void KBD_Close();

#endif /* KBD_H */
