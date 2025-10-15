#include  <smoke.h>

extern worker_data *worker;

void load_texture_web(Uint8 *texture_png, Uint64 texture_png_len) {
	SDL_IOStream *stream = SDL_IOFromConstMem(texture_png, texture_png_len);
	SDL_Surface *surface = IMG_Load_IO(stream, true);
	worker->window->texture = SDL_CreateTextureFromSurface(worker->window->renderer, surface);
	if (!surface || !worker->window->texture) exit(1);
	SDL_DestroySurface(surface);
}
