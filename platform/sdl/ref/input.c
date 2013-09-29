#include "../ref_gl/gl_local.h"
#include "../client/keys.h"
#include "rw_sdl.h"

#include <SDL.h>
#include <SDL_scancode.h>

extern SDL_Window *_gl_window;

static int   mx, my;
static int	old_mouse_x, old_mouse_y;

static int win_x, win_y;

static cvar_t	*m_filter;
static cvar_t	*in_mouse;
static cvar_t	*in_dgamouse;

static cvar_t	*r_fakeFullscreen;

static int num_vidmodes;
static qboolean vidmode_active = false;
static qboolean	mlooking;
static qboolean vidmode_ext = false;

// state struct passed in Init
static in_state_t	*in_state;

static cvar_t *sensitivity;
static cvar_t *lookstrafe;
static cvar_t *m_side;
static cvar_t *m_yaw;
static cvar_t *m_pitch;
static cvar_t *m_forward;
static cvar_t *freelook;

static qboolean mouse_grabbed = false; // This is also mouse_active

#include <stdio.h>
#define MAP(a, b) case a: return b;
static int _TranslateKey(SDL_Keysym *keysym) {
	// Map scancode-based keys first
	switch (keysym->scancode) {
		MAP(SDL_SCANCODE_UP, K_UPARROW);
		MAP(SDL_SCANCODE_LEFT, K_LEFTARROW);
		MAP(SDL_SCANCODE_RIGHT, K_RIGHTARROW);
		MAP(SDL_SCANCODE_DOWN, K_DOWNARROW);
		
		MAP(SDL_SCANCODE_PAGEUP, K_PGUP);
		MAP(SDL_SCANCODE_PAGEDOWN, K_PGDN);
		
		MAP(SDL_SCANCODE_HOME, K_HOME);
		MAP(SDL_SCANCODE_END, K_END);
		
		MAP(SDL_SCANCODE_RETURN, K_ENTER);
		MAP(SDL_SCANCODE_KP_ENTER, K_KP_ENTER);
		
		MAP(SDL_SCANCODE_TAB, K_TAB);
		MAP(SDL_SCANCODE_ESCAPE, K_ESCAPE);
		
		MAP(SDL_SCANCODE_F1, K_F1);
		MAP(SDL_SCANCODE_F2, K_F2);
		MAP(SDL_SCANCODE_F3, K_F3);
		MAP(SDL_SCANCODE_F4, K_F4);
		MAP(SDL_SCANCODE_F5, K_F5);
		MAP(SDL_SCANCODE_F6, K_F6);
		MAP(SDL_SCANCODE_F7, K_F7);
		MAP(SDL_SCANCODE_F8, K_F8);
		MAP(SDL_SCANCODE_F9, K_F9);
		MAP(SDL_SCANCODE_F10, K_F10);
		MAP(SDL_SCANCODE_F11, K_F11);
		MAP(SDL_SCANCODE_F12, K_F12);
		
		MAP(SDL_SCANCODE_BACKSPACE, K_BACKSPACE);
		
		MAP(SDL_SCANCODE_INSERT, K_INS);
		MAP(SDL_SCANCODE_DELETE, K_DEL);
		
		MAP(SDL_SCANCODE_PAUSE, K_PAUSE);
		
		MAP(SDL_SCANCODE_LSHIFT, K_SHIFT);
		MAP(SDL_SCANCODE_RSHIFT, K_SHIFT);
		
		MAP(SDL_SCANCODE_LCTRL, K_CTRL);
		MAP(SDL_SCANCODE_RCTRL, K_CTRL);
		
		MAP(SDL_SCANCODE_LALT, K_ALT);
		MAP(SDL_SCANCODE_RALT, K_ALT);
		MAP(SDL_SCANCODE_LGUI, K_ALT);
		MAP(SDL_SCANCODE_RGUI, K_ALT);
		
		MAP(SDL_SCANCODE_KP_MULTIPLY, '*');
		MAP(SDL_SCANCODE_KP_PLUS, K_KP_PLUS);
		MAP(SDL_SCANCODE_KP_MINUS, K_KP_MINUS);
		MAP(SDL_SCANCODE_KP_DIVIDE, K_KP_SLASH);
		
		MAP(SDL_SCANCODE_SPACE, K_SPACE);
		
		MAP(SDL_SCANCODE_KP_1, '1');
		MAP(SDL_SCANCODE_KP_2, '2');
		MAP(SDL_SCANCODE_KP_3, '3');
		MAP(SDL_SCANCODE_KP_4, '4');
		MAP(SDL_SCANCODE_KP_5, '5');
		MAP(SDL_SCANCODE_KP_6, '6');
		MAP(SDL_SCANCODE_KP_7, '7');
		MAP(SDL_SCANCODE_KP_8, '8');
		MAP(SDL_SCANCODE_KP_9, '9');
		MAP(SDL_SCANCODE_KP_0, '0');
		default:
			break;
	}
	
	if (keysym->sym == 0xb4)
		return '`';
	
	return keysym->sym;
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
			//if (in_state && in_state->Key_Event_fp)
				in_state->Key_Event_fp(_TranslateKey(&event.key.keysym), event.key.state == SDL_PRESSED);
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

		case SDL_MOUSEBUTTONUP:
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
		case SDL_QUIT:
			ri.Cmd_ExecuteText(0, "quit");
			break;
		}
	}
}

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp) {
	Key_Event_fp = fp;
}

void KBD_Update() {
	HandleEvents();
}

void KBD_Close() {
}

void install_grabs() {
	SDL_SetWindowGrab(_gl_window, true);
	SDL_SetRelativeMouseMode(true);
	mouse_grabbed = true;
}

void uninstall_grabs() {
	SDL_SetWindowGrab(_gl_window, false);
	SDL_SetRelativeMouseMode(false);
	mouse_grabbed = false;
}

static void Force_CenterView_f() {
	in_state->viewangles[PITCH] = 0;
}

static void RW_IN_MLookDown() {
	mlooking = true; 
}

static void RW_IN_MLookUp() {
	mlooking = false;
	in_state->IN_CenterView_fp ();
}

void RW_IN_Init(in_state_t *in_state_p) {
	int mtype;
	int i;

	in_state = in_state_p;

	// mouse variables
	m_filter = ri.Cvar_Get ("m_filter", "0", 0);
    in_mouse = ri.Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
	freelook = ri.Cvar_Get( "freelook", "0", 0 );
	lookstrafe = ri.Cvar_Get ("lookstrafe", "0", 0);
	sensitivity = ri.Cvar_Get ("sensitivity", "10", 0);
	m_pitch = ri.Cvar_Get ("m_pitch", "0.022", 0);
	m_yaw = ri.Cvar_Get ("m_yaw", "0.022", 0);
	m_forward = ri.Cvar_Get ("m_forward", "1", 0);
	m_side = ri.Cvar_Get ("m_side", "0.8", 0);

	ri.Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
	ri.Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

	ri.Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	mx = my = 0.0;
}

void RW_IN_Shutdown() {
}

void RW_IN_Commands() {
}

void RW_IN_Move (usercmd_t *cmd) {
	if (mouse_grabbed == false)
		return;
	if (m_filter->value)
	{
		mx = (mx + old_mouse_x) * 0.5;
		my = (my + old_mouse_y) * 0.5;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mx *= sensitivity->value;
	my *= sensitivity->value;

// add mouse X/Y movement to cmd
	if ( (*in_state->in_strafe_state & 1) || 
		(lookstrafe->value && mlooking ))
		cmd->sidemove += m_side->value * mx;
	else
		in_state->viewangles[YAW] -= m_yaw->value * mx;

	if ( (mlooking || freelook->value) && 
		!(*in_state->in_strafe_state & 1))
	{
		in_state->viewangles[PITCH] += m_pitch->value * my;
	}
	else
	{
		cmd->forwardmove -= m_forward->value * my;
	}

	mx = my = 0;
}

static void IN_DeactivateMouse() {
	uninstall_grabs();
}

static void IN_ActivateMouse() {
	mx = my = 0; // don't spazz
	install_grabs();
}

void RW_IN_Frame() {
}

void RW_IN_Activate(qboolean active) {
	if (active || vidmode_active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse ();
}
