#pragma once

#include <iostream>
#include <unistd.h>
#include <SDL3/SDL.h>

#define BGC 0xfffcf2ff
#define FGC 0x252433ff
#define FPS 60
#define NANOS_PER_SECOND 0x3b9aca00
#define NANOS_PER_FRAME NANOS_PER_SECOND/FPS
#define DEF_WIN_WIDTH 0x500
#define DEF_WIN_HEIGHT 0x280
#define MIN_WIN_WIDTH 0xc8
#define MIN_WIN_HEIGHT 0x64

typedef struct graphic {
	SDL_Window	*window;
	SDL_Renderer	*renderer;
	Uint16		win_height;
	Uint16		win_width;
} win;

typedef struct worker {
	bool		halt;
	pthread_t		worker;
	pthread_mutex_t	halt_mutex;
	win		*window;
} worker_data;
