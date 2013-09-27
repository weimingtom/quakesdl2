// TODO UNIFY remove
#include "../client/client.h"
#include "../client/snd_loc.h"

void S_WriteLinearBlastStereo16() {
}

void S_PaintChannelFrom8() {
}

#if 0
qboolean SNDDMA_Init() {
	cvar_t *sndbits;
	cvar_t *sndspeed;
	cvar_t *sndchannels;

	sndbits = Cvar_Get("sndbits", "16", CVAR_ARCHIVE);
	sndspeed = Cvar_Get("sndspeed", "0", CVAR_ARCHIVE);
	sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
	
	dma.samplebits = (int)sndbits->value;
	dma.speed = (int)sndspeed->value;
	dma.channels = (int)sndchannels->value;
		
	return true;
}

int SNDDMA_GetDMAPos() {
}

void SNDDMA_Shutdown() {
}

void SNDDMA_Submit() {
}

void SNDDMA_BeginPainting() {
}

#endif
