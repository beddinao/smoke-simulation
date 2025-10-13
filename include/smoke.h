#pragma once

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>

#define BGC		0x000000ff
#define FGC		0xff0000ff
#define BRC		0x0000ffff
#define FPS		60
#define NANOS_PER_SECOND 	0x3b9aca00
#define NANOS_PER_FRAME	NANOS_PER_SECOND/FPS
#define DEF_RECT_WIDTH	50
#define DEF_RECT_HEIGHT	50
#define MAX_RECTS		0x64
#define DEF_WIN_WIDTH	0x500
#define DEF_WIN_HEIGHT	0x280
#define MIN_WIN_WIDTH	0xc8
#define MIN_WIN_HEIGHT	0x64
#define ALPHA_INDX		3
#define VELOCITY_X_INDX	0.005
#define VELOCITY_Y_INDX	0.01
#define SCALE_W_INDX	7
#define SCALE_H_INDX	7
#define RECT_MAX_WIDTH	DEF_WIN_WIDTH/3
#define RECT_MAX_HEIGHT	DEF_WIN_WIDTH/3
#define Y_FIX_VELOCITY	8

#define TEXTURE_PATH "assets/smoke_texture_2.png"

typedef struct angle {
	int16_t		angle;
	int8_t		dir;
	int8_t		indx;
} angle;

typedef struct graphic {
	SDL_Window	*window;
	SDL_Renderer	*renderer;
	Uint16		win_height;
	Uint16		win_width;
	SDL_FRect		rects[MAX_RECTS];
	Uint8		alphas[MAX_RECTS];
	angle		angles[MAX_RECTS];
	float		velosX[MAX_RECTS];
	float		velosY[MAX_RECTS];
	SDL_Texture	*texture;	
	bool		mouse_active;
	Uint8		mouse_iter;
	Uint32		rIndex;
} win;

typedef struct worker {
	bool		halt;
	pthread_t		worker;
	pthread_mutex_t	halt_mutex;
	pthread_mutex_t	data_mutex;
	pthread_mutex_t	indx_mutex;
	win		*window;
} worker_data;
