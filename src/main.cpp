#include <smoke.h>

void *animation_routine(void *p) {
	worker_data *worker = (worker_data*)p;
	for(;;) {
		pthread_mutex_lock(&worker->halt_mutex);
		if (worker->halt) {
			pthread_mutex_unlock(&worker->halt_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&worker->halt_mutex);
	}
	return NULL;
}

void draw_routine(worker_data*worker) {
	struct timespec frame_start_time, frame_end_time;
	Uint64 elapsed_nanoseconds;
	SDL_Event event;
	Uint32 rIndex(0);
	clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
	for (;;) {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					std::cout << "SDL_QUIT success" << std::endl;
					pthread_mutex_lock(&worker->halt_mutex);
					worker->halt = true;
					pthread_mutex_unlock(&worker->halt_mutex);
					return;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					worker->window->alphas[rIndex] = 0xff;
					worker->window->rects[rIndex].x = event.button.x-DEF_RECT_WIDTH/2;
					worker->window->rects[rIndex].y = event.button.y-DEF_RECT_HEIGHT/2;
					worker->window->rects[rIndex].w = DEF_RECT_WIDTH;
					worker->window->rects[rIndex++].h = DEF_RECT_HEIGHT;
					if (rIndex >= MAX_RECTS)
						rIndex = 0;
					break;
				default: break;
			}
		}
		clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
		elapsed_nanoseconds = (frame_end_time.tv_sec-frame_start_time.tv_sec) * NANOS_PER_SECOND
			+ (frame_end_time.tv_nsec-frame_start_time.tv_nsec);
		if (elapsed_nanoseconds < NANOS_PER_FRAME)
			continue;
		else clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
		SDL_SetRenderDrawColor(worker->window->renderer,
				(BGC>>24)&0xff,(BGC>>16)&0xff,
				(BGC>>8)&0xff,BGC&0xff);
		SDL_RenderClear(worker->window->renderer);
		/*SDL_SetRenderDrawColor(worker->window->renderer,
				(FGC>>24)&0xff,(FGC>>16)&0xff,
				(FGC>>8)&0xff,FGC&0xff);
		SDL_RenderFillRects(worker->window->renderer, worker->window->rects, rIndex);
		SDL_SetRenderDrawColor(worker->window->renderer,
				(BRC>>24)&0xff,(BRC>>16)&0xff,
				(BRC>>8)&0xff,BRC&0xff);
		SDL_RenderRects(worker->window->renderer, worker->window->rects, rIndex);*/
		for (Uint32 current_rect = 0; current_rect < rIndex; current_rect++)
			SDL_RenderTexture(worker->window->renderer, worker->window->texture,
					NULL, &worker->window->rects[current_rect]);
		SDL_RenderPresent(worker->window->renderer);
	}
}

bool init_window(worker_data *worker) {
	win *window = worker->window;
	SDL_Window *sdl_win = NULL;
	SDL_Renderer *renderer =  NULL;
	window->win_height = DEF_WIN_HEIGHT;
	window->win_width = DEF_WIN_WIDTH;
	if (!SDL_Init(SDL_INIT_EVENTS))
		return false;
	sdl_win = SDL_CreateWindow("smoking",
			window->win_width,
			window->win_height, 0);
	renderer = SDL_CreateRenderer(sdl_win, NULL);
	if (!sdl_win || !renderer) return false;
	SDL_SetWindowMinimumSize(sdl_win,
			MIN_WIN_WIDTH,
			MIN_WIN_HEIGHT);
	SDL_Surface *surface = IMG_Load(TEXTURE_PATH);
	if (!surface || !(worker->window->texture = SDL_CreateTextureFromSurface(
			renderer, surface))) exit(1);
	window->window = sdl_win;
	window->renderer = renderer;
	SDL_DestroySurface(surface);
	return true;
}

int main() {
	srand(time(NULL));
	worker_data *worker = (worker_data*)malloc(sizeof(worker_data));
	win *window = (win*)malloc(sizeof(win));
	if (!worker || !window) exit(1);
	memset(worker, 0, sizeof(worker_data));
	memset(window, 0, sizeof(window));
	worker->window = window;
	if (!init_window(worker)) exit(1);
	pthread_mutex_init(&worker->halt_mutex, NULL);
	pthread_create(&worker->worker, NULL, animation_routine, worker);
	draw_routine(worker);
	pthread_join(worker->worker, NULL);
	pthread_mutex_destroy(&worker->halt_mutex);
	SDL_DestroyTexture(worker->window->texture);
	SDL_DestroyRenderer(worker->window->renderer);
	SDL_DestroyWindow(worker->window->window);
	SDL_Quit();
	free(worker->window);
	free(worker);
}
