#include "kbd.h"

void (*KBD_Init_fp)(Key_Event_fp_t fp) = NULL;
void (*KBD_Update_fp)(void) = NULL;
void (*KBD_Close_fp)(void) = NULL;

void KBD_Init(Key_Event_fp_t fp) {
	(*KBD_Init_fp)(fp);
}

void KBD_Update() {
	(*KBD_Update_fp)();
}

void KBD_Close() {
	(*KBD_Close_fp)();
}
