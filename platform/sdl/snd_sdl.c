#include <SDL.h>

#include "../client/client.h"
#include "../client/snd_loc.h"

static SDL_AudioDeviceID _snd_audioId = 0;
static SDL_AudioSpec obtained;

/* Convert bytes to the number of samples you can fit in there. Always consider
 * the buffer holding only a mono channel, SND will do the right thing */
#define SNDDMA_BYTESTOMSAMPLES(a) ((a)/(dma.samplebits/8))
#define SNDDMA_MSAMPLESTOBYTES(a) ((a)*(dma.samplebits/8))

static int _snd_bufferSize; // DMA buffer size
static int _snd_bufferPos; // Current playing position, in bytes

// Valid audio device settings. First one is default
static int _snd_allowedFreqs[] = { 44100, 22050, 8000, 11025 }; // 11025 doesnt really work with SDL2 2.0.0
static SDL_AudioFormat _snd_allowedFormats[] = { AUDIO_S16LSB, AUDIO_U8 };
static const unsigned int _snd_numAllowedFreqs = 4;
static const unsigned int _snd_numAllowedFormats = 2;

/* Utility */
static int _SNDDMA_NextPow2(int a) {
	int i = 1;
	while (i < a)
		i = i * 2;
}

static qboolean _SNDDMA_MapAudioSpec2DMA(const SDL_AudioSpec *spec, dma_t *dma) {
	dma->channels = spec->channels;
	dma->speed = spec->freq;
	if (spec->format == AUDIO_S16LSB)
		dma->samplebits = 16;
	else if (spec->format == AUDIO_U8)
		dma->samplebits = 8;
	else
		return false;
	dma->submission_chunk = 1;
	
	return true;
}

/* Main stuff */
// This may get called in a seperate thread. Locking is done in SNDDMA_*
static void _SNDDMA_AudioCallback(void* userdata, Uint8* stream, int len) {
	/* Just blindly read as much data as requested. Works as long as theres
	 * no overflow, which we can't handle anyway */
	int i;

	for (i = 0; i < len; i++) {
		stream[i] = dma.buffer[_snd_bufferPos];
		_snd_bufferPos++;
		if (_snd_bufferPos >= _snd_bufferSize)
			_snd_bufferPos = 0;
	}
}

qboolean SNDDMA_Init() {
	cvar_t *snd_sdlresample = Cvar_Get("snd_sdlresample", "0", CVAR_ARCHIVE);
	qboolean sdlresample = false;
	if (strcmp(snd_sdlresample->string, "1") == 0)
		sdlresample = true;
	
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	
	SDL_AudioSpec desired;
	
	memset(&desired, 0, sizeof(SDL_AudioSpec));
	desired.freq = _snd_allowedFreqs[0];
	desired.format = _snd_allowedFormats[0];
	desired.channels = 2;
	desired.samples = 4096; // Buffer size, good default
	desired.callback = _SNDDMA_AudioCallback;
	
	memset(&obtained, 0, sizeof(SDL_AudioSpec));
	
	int allowed_changes = 0;
	if (sdlresample == false)
		allowed_changes = SDL_AUDIO_ALLOW_ANY_CHANGE;
	
	_snd_audioId = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, allowed_changes);
	if (_snd_audioId == 0) {
		Com_Printf("Can't open default audio device.");
		return false;
	}
	
	// Check if we got a valid setting when handling resampling ourselves
	if (sdlresample == false) {
		int i;
		for (i = 0; i < _snd_numAllowedFreqs; i++)
			if (obtained.freq == _snd_allowedFreqs[i])
				break;
		if (i >= _snd_numAllowedFreqs) {
			Com_Printf("Got invalid sampling frequency: %d. Try setting \"snd_sdlresample\" to 1.\n");
			goto error;
		}
		for (i = 0; i < _snd_numAllowedFormats; i++)
			if (obtained.format == _snd_allowedFormats[i])
				break;
		if (i >= _snd_numAllowedFormats) {
			Com_Printf("Got invalid sample format. Try setting \"snd_sdlresample\" to 1.\n");
			goto error;
		}
		if (obtained.channels != 2) {
			Com_Printf("Your sound hardware doesn't seem to support stereo. Try setting \"snd_sdlresample\" to 1.\n");
			goto error;
		}
	}
	
	memset(&dma, 0, sizeof(dma_t));
	if (_SNDDMA_MapAudioSpec2DMA(&obtained, &dma) == false)
		goto error;
	
	/* Find a suitable size for DMA buffer. Use the next biggest power 
	 * of 2(non pow2 don't work at all for some reason. Haven't looked yet why) */
	_snd_bufferSize = _SNDDMA_NextPow2(obtained.size*4);
	
	// Init DMA buffer
	dma.samples = SNDDMA_BYTESTOMSAMPLES(_snd_bufferSize);
	dma.buffer = malloc(_snd_bufferSize);
	memset(dma.buffer, 0, _snd_bufferSize);
	
	_snd_bufferPos = 0;
	
	SDL_PauseAudioDevice(_snd_audioId, 0);
	return true;
	
	error:
	SDL_CloseAudioDevice(_snd_audioId);
	_snd_audioId = 0;
	return false;
}

void SNDDMA_Shutdown() {
	free(dma.buffer);
	SDL_CloseAudioDevice(_snd_audioId);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	
	_snd_audioId = 0;
}

int SNDDMA_GetDMAPos() {
	SDL_LockAudioDevice(_snd_audioId);
	
	int ret = SNDDMA_BYTESTOMSAMPLES(_snd_bufferPos);
	
	SDL_UnlockAudioDevice(_snd_audioId);
	
	return ret;
}

void SNDDMA_BeginPainting() {
	SDL_LockAudioDevice(_snd_audioId);
}

void SNDDMA_Submit() {
	SDL_UnlockAudioDevice(_snd_audioId);
}
