#include  <smoke.h>

extern worker_data *worker;

void load_texture_web(uint8_t *texture_png, size_t texture_png_len) {
	printf("got len: %zu\n", texture_png_len);
	if (worker->window->texture != NULL) {
		printf("texture already loaded\n");
		return;
	}
	SDL_IOStream *stream = SDL_IOFromConstMem(texture_png, texture_png_len-1);
	SDL_Surface *surface = IMG_Load_IO(stream, true);
	worker->window->texture = SDL_CreateTextureFromSurface(worker->window->renderer, surface);
	if (!surface || !worker->window->texture) exit(1);
	SDL_DestroySurface(surface);
}
