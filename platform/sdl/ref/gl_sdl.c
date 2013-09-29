/* WARNING: Outdated information:
 * This file contains ALL Linux specific stuff having to do with the
 * OpenGL refresh.  When a port is being made the following functions
 * must be implemented by the port:
 *
 * GLimp_EndFrame
 * GLimp_Init
 * GLimp_Shutdown
 * GLimp_SwitchFullscreen */

#include <SDL.h>
#include <SDL_video.h>
#include "../ref_gl/gl_local.h"

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )

SDL_Window *_gl_window = NULL;
static SDL_GLContext _gl_context = NULL;

/*****************************************************************************/

static void _DestroyWindow() {
	if (_gl_window) {
		SDL_GL_DeleteContext(_gl_context);
		SDL_DestroyWindow(_gl_window);
		_gl_window = NULL;
		_gl_context = NULL;
	}
}

int GLimp_SetMode(int *pwidth, int *pheight, int mode, qboolean fullscreen) {
	Uint32 windowflags = SDL_WINDOW_OPENGL;
	cvar_t *vid_xpos = ri.Cvar_Get("vid_xpos", "0", CVAR_ARCHIVE);
	cvar_t *vid_ypos = ri.Cvar_Get("vid_ypos", "0", CVAR_ARCHIVE);
	
	_DestroyWindow();
	
	ri.Vid_GetModeInfo(pwidth, pheight, mode);
	
	if (fullscreen)
		windowflags |= SDL_WINDOW_FULLSCREEN;
	_gl_window = SDL_CreateWindow("SDLQuake", vid_xpos->value, vid_ypos->value, *pwidth, *pheight, windowflags);
	if (_gl_window == NULL)
		return rserr_invalid_mode;
	_gl_context = SDL_GL_CreateContext(_gl_window);
	if (_gl_context == NULL)
		return rserr_invalid_mode;
	SDL_GL_MakeCurrent(_gl_window, _gl_context);

	// Let the sound and input subsystems know about the new window
	ri.Vid_NewWindow(*pwidth, *pheight);

	return rserr_ok;
}

/* This routine does all OS specific shutdown procedures for the OpenGL
 * subsystem.  Under OpenGL this means NULLing out the current DC and
 * HGLRC, deleting the rendering context, and releasing the DC acquired
 * for the window.  The state structure is also nulled out. */
void GLimp_Shutdown() {
	_DestroyWindow();
	
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/* This routine is responsible for initializing the OS specific portions
 * of OpenGL.  */
int GLimp_Init(void *hinstance, void *wndproc) {
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	return true;
}

void GLimp_BeginFrame(float camera_seperation) {
}

/* Responsible for doing a swapbuffers and possibly for other stuff
 * as yet to be determined.  Probably better not to make this a GLimp
 * function and instead do a call to GLimp_SwapBuffers. */
void GLimp_EndFrame() {
	SDL_GL_SwapWindow(_gl_window);
}

void GLimp_AppActivate(qboolean active) {
}

void Fake_glColorTableEXT( GLenum target, GLenum internalformat,
                             GLsizei width, GLenum format, GLenum type,
                             const GLvoid *table )
{
	byte temptable[256][4];
	byte *intbl;
	int i;

	for (intbl = (byte *)table, i = 0; i < 256; i++) {
		temptable[i][2] = *intbl++;
		temptable[i][1] = *intbl++;
		temptable[i][0] = *intbl++;
		temptable[i][3] = 255;
	}
	qgl3DfxSetPaletteEXT((GLuint *)temptable);
}
