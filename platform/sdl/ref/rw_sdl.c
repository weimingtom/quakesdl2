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
/*
** RW_X11.C
**
** This file contains ALL Linux specific stuff having to do with the
** software refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** SWimp_EndFrame
** SWimp_Init
** SWimp_InitGraphics
** SWimp_SetPalette
** SWimp_Shutdown
** SWimp_SwitchFullscreen
*/

#include <SDL.h>
#include <SDL_video.h>

#include "../ref_soft/r_local.h"

#include <stdio.h>

SDL_Window *_ref_window = NULL;
static SDL_Surface *_soft_surface = NULL;
static SDL_Surface *_soft_winsurface = NULL; // Do not free
static SDL_Palette *_soft_palette = NULL;

#define freeMaybe(a, f) if ((a) != NULL) { (f)((a)); (a) = NULL; }
static void _DestroyWindow() {
	freeMaybe(_soft_surface, SDL_FreeSurface);
	freeMaybe(_soft_palette, SDL_FreePalette);
	freeMaybe(_ref_window, SDL_DestroyWindow);
}

/* This routine is responsible for initializing the implementation
 * specific stuff in a software rendering subsystem. */
int SWimp_Init(void *hInstance, void *wndProc) {
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		SDL_Init(SDL_INIT_VIDEO);
}

/* This does an implementation specific copy from the backbuffer to the
 * front buffer.  In the Win32 case it uses BitBlt or BltFast depending
 * on whether we're using DIB sections/GDI or DDRAW. */
void SWimp_EndFrame() {
	/* We need to convert the surface ourselves here because SDL sucks.
	 * It won't notice a palette change after the first BlitSurface call, so
	 * everything that uses it's own palette would get screwed up, like movies.
	 * If you know of any way to fix that, feel free to implement it. Saves
	 * a copy on each frame */
	SDL_Surface *tmp = SDL_ConvertSurface(_soft_surface, SDL_GetWindowSurface(_ref_window)->format, 0);
	SDL_BlitSurface(tmp, NULL, SDL_GetWindowSurface(_ref_window), NULL);
	SDL_FreeSurface(tmp);
	
	SDL_UpdateWindowSurface(_ref_window);
}

rserr_t SWimp_SetMode(int *pwidth, int *pheight, int mode, qboolean fullscreen) {
	Uint32 windowflags = 0;
	
	_DestroyWindow();
	
	ri.Vid_GetModeInfo(pwidth, pheight, mode);
	cvar_t *vid_xpos = ri.Cvar_Get("vid_xpos", "50", CVAR_ARCHIVE);
	cvar_t *vid_ypos = ri.Cvar_Get("vid_ypos", "50", CVAR_ARCHIVE);
	if (fullscreen == true)
		windowflags |= SDL_WINDOW_FULLSCREEN;
	
	_ref_window = SDL_CreateWindow("SDLQuake Soft", vid_xpos->value, vid_ypos->value, *pwidth, *pheight, windowflags);
	/* For some reason, the following procedure is needed in order to enable mouse grabbing.
	 * Of SDL_GetWindowSurface ever get's called before processing one event, grabbing
	 * will silently fail.
	 * I doubt this is the correct solution, it probably just updates something
	 * somewhere for us. But it works, so whatever... */
	SDL_Event event;
	SDL_WaitEvent(&event);
	_soft_winsurface = SDL_GetWindowSurface(_ref_window);

	_soft_surface = SDL_CreateRGBSurface(0, *pwidth, *pheight, 8, 0, 0, 0, 0);
	if (_soft_surface == NULL)
		return rserr_invalid_mode;
	
	vid.width = *pwidth;
	vid.height = *pheight;
	vid.buffer = _soft_surface->pixels;
	vid.rowbytes = _soft_surface->pitch;
	
	// Let the sound and input subsystems know about the new window
	ri.Vid_NewWindow(*pwidth, *pheight);
	
	return rserr_ok;
}

/* System specific palette setting routine.  A NULL palette means
 * to use the existing palette.  The palette is expected to be in
 * a padded 4-byte xRGB format. */
void SWimp_SetPalette(const unsigned char *palette) {
	if (_soft_surface == NULL)
		return;
		
	if (_soft_palette != NULL)
		SDL_FreePalette(_soft_palette);
	
	uint32_t *data = palette;
	if (palette == NULL)
		data = sw_state.currentpalette;
		
	_soft_palette = SDL_AllocPalette(256);
	int i;
	for (i = 0; i < 256; i++) {
		_soft_palette->colors[i].r = (data[i] >> 0) & 0xFF;
		_soft_palette->colors[i].g = (data[i] >> 8) & 0xFF;
		_soft_palette->colors[i].b = (data[i] >> 16) & 0xFF;
		_soft_palette->colors[i].a = 0;
	}
	
	SDL_SetSurfacePalette(_soft_surface, _soft_palette);
}

/* System specific graphics subsystem shutdown routine.  Destroys
 * DIBs or DDRAW surfaces as appropriate. */
void SWimp_Shutdown() {
	_DestroyWindow();
	
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SWimp_AppActivate(qboolean active) {
}


void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length) {
	int r;
	unsigned long addr;
	int psize = getpagesize();

	addr = (startaddr & ~(psize-1)) - psize;
	r = mprotect((char*)addr, length + startaddr - addr + psize, 7);

	if (r < 0)
		Sys_Error("Protection change failed\n");
}
