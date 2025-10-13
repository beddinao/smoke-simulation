#pragma once

#include <iostream>
#include <unistd.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>

#define BGC 0x000000ff
#define FGC 0xff0000ff
#define BRC 0x0000ffff
#define FPS 60
#define NANOS_PER_SECOND 0x3b9aca00
#define NANOS_PER_FRAME NANOS_PER_SECOND/FPS
#define DEF_RECT_WIDTH 100
#define DEF_RECT_HEIGHT 100
#define MAX_RECTS 0xff
#define DEF_WIN_WIDTH 0x500
#define DEF_WIN_HEIGHT 0x280
#define MIN_WIN_WIDTH 0xc8
#define MIN_WIN_HEIGHT 0x64

#define TEXTURE_PATH "assets/smoke_texture.png"

typedef struct graphic {
	SDL_Window	*window;
	SDL_Renderer	*renderer;
	Uint16		win_height;
	Uint16		win_width;
	Uint8		alphas[MAX_RECTS];
	SDL_FRect		rects[MAX_RECTS];
	SDL_Texture	*texture;	
} win;

typedef struct worker {
	bool		halt;
	pthread_t		worker;
	pthread_mutex_t	halt_mutex;
	win		*window;
} worker_data;
