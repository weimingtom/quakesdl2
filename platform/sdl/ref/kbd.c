#include "../ref_gl/gl_local.h"
#include "../client/keys.h"

#include <SDL.h>

static int _TranslateKey(SDL_Keysym *keysym) {
	return 0;
#if 0
	int key;
	char buf[64];
	KeySym keysym;

	key = 0;

	XLookupString(ev, buf, sizeof buf, &keysym, 0);

	switch(keysym)
	{
		case XK_KP_Page_Up:	 key = K_KP_PGUP; break;
		case XK_Page_Up:	 key = K_PGUP; break;

		case XK_KP_Page_Down: key = K_KP_PGDN; break;
		case XK_Page_Down:	 key = K_PGDN; break;

		case XK_KP_Home: key = K_KP_HOME; break;
		case XK_Home:	 key = K_HOME; break;

		case XK_KP_End:  key = K_KP_END; break;
		case XK_End:	 key = K_END; break;

		case XK_KP_Left: key = K_KP_LEFTARROW; break;
		case XK_Left:	 key = K_LEFTARROW; break;

		case XK_KP_Right: key = K_KP_RIGHTARROW; break;
		case XK_Right:	key = K_RIGHTARROW;		break;

		case XK_KP_Down: key = K_KP_DOWNARROW; break;
		case XK_Down:	 key = K_DOWNARROW; break;

		case XK_KP_Up:   key = K_KP_UPARROW; break;
		case XK_Up:		 key = K_UPARROW;	 break;

		case XK_Escape: key = K_ESCAPE;		break;

		case XK_KP_Enter: key = K_KP_ENTER;	break;
		case XK_Return: key = K_ENTER;		 break;

		case XK_Tab:		key = K_TAB;			 break;

		case XK_F1:		 key = K_F1;				break;

		case XK_F2:		 key = K_F2;				break;

		case XK_F3:		 key = K_F3;				break;

		case XK_F4:		 key = K_F4;				break;

		case XK_F5:		 key = K_F5;				break;

		case XK_F6:		 key = K_F6;				break;

		case XK_F7:		 key = K_F7;				break;

		case XK_F8:		 key = K_F8;				break;

		case XK_F9:		 key = K_F9;				break;

		case XK_F10:		key = K_F10;			 break;

		case XK_F11:		key = K_F11;			 break;

		case XK_F12:		key = K_F12;			 break;

		case XK_BackSpace: key = K_BACKSPACE; break;

		case XK_KP_Delete: key = K_KP_DEL; break;
		case XK_Delete: key = K_DEL; break;

		case XK_Pause:	key = K_PAUSE;		 break;

		case XK_Shift_L:
		case XK_Shift_R:	key = K_SHIFT;		break;

		case XK_Execute: 
		case XK_Control_L: 
		case XK_Control_R:	key = K_CTRL;		 break;

		case XK_Alt_L:	
		case XK_Meta_L: 
		case XK_Alt_R:	
		case XK_Meta_R: key = K_ALT;			break;

		case XK_KP_Begin: key = K_KP_5;	break;

		case XK_Insert:key = K_INS; break;
		case XK_KP_Insert: key = K_KP_INS; break;

		case XK_KP_Multiply: key = '*'; break;
		case XK_KP_Add:  key = K_KP_PLUS; break;
		case XK_KP_Subtract: key = K_KP_MINUS; break;
		case XK_KP_Divide: key = K_KP_SLASH; break;

#if 0
		case 0x021: key = '1';break;/* [!] */
		case 0x040: key = '2';break;/* [@] */
		case 0x023: key = '3';break;/* [#] */
		case 0x024: key = '4';break;/* [$] */
		case 0x025: key = '5';break;/* [%] */
		case 0x05e: key = '6';break;/* [^] */
		case 0x026: key = '7';break;/* [&] */
		case 0x02a: key = '8';break;/* [*] */
		case 0x028: key = '9';;break;/* [(] */
		case 0x029: key = '0';break;/* [)] */
		case 0x05f: key = '-';break;/* [_] */
		case 0x02b: key = '=';break;/* [+] */
		case 0x07c: key = '\'';break;/* [|] */
		case 0x07d: key = '[';break;/* [}] */
		case 0x07b: key = ']';break;/* [{] */
		case 0x022: key = '\'';break;/* ["] */
		case 0x03a: key = ';';break;/* [:] */
		case 0x03f: key = '/';break;/* [?] */
		case 0x03e: key = '.';break;/* [>] */
		case 0x03c: key = ',';break;/* [<] */
#endif

		default:
			key = *(unsigned char*)buf;
			if (key >= 'A' && key <= 'Z')
				key = key - 'A' + 'a';
			break;
	} 

	return key;
#endif
}


static void HandleEvents() {
	SDL_Event event;
	int b;
	qboolean dowarp = false;
	int mwx = vid.width/2;
	int mwy = vid.height/2;
   
	while (SDL_PollEvent(&event) != 0) {
		switch(event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (in_state && in_state->Key_Event_fp)
				in_state->Key_Event_fp(_TranslateKey(event.key.keysym), event.type == KeyPress);
			break;

		case SDL_MOUSEMOTION:
			mx = event.motion.xrel;
			my = event.motion.yrel;
			break;
		case SDL_MOUSEBUTTONDOWN:
			b=-1;
			if (event.button.button = SDL_BUTTON_LEFT)
				b = 0;
			else if (event.button.button == SDL_BUTTON_RIGHT)
				b = 2;
			else if (event.button.button == SDL_BUTTON_MIDDLE)
				b = 1;
			if (b>=0 && in_state && in_state->Key_Event_fp)
				in_state->Key_Event_fp(K_MOUSE1 + b, true);
			break;

		case ButtonRelease:
			b=-1;
			if (event.button.button = SDL_BUTTON_LEFT)
				b = 0;
			else if (event.button.button == SDL_BUTTON_RIGHT)
				b = 2;
			else if (event.button.button == SDL_BUTTON_MIDDLE)
				b = 1;
			if (b>=0 && in_state && in_state->Key_Event_fp)
				in_state->Key_Event_fp(K_MOUSE1 + b, false);
			break;
		}
	}
	   
	if (dowarp) {
		/* move the mouse to the window center again */
		//XWarpPointer(dpy,None,win,0,0,0,0, vid.width/2,vid.height/2);
	}
}

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
	Key_Event_fp = fp;
}

void KBD_Update(void)
{
// get events from x server
	HandleEvents();
}

void KBD_Close(void)
{
}
