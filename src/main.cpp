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
	clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
	for (;;) {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					pthread_mutex_lock(&worker->halt_mutex);
					worker->halt = true;
					pthread_mutex_unlock(&worker->halt_mutex);
					return;
				default: break;
			}
		}
		clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
		elapsed_nanoseconds = (frame_end_time.tv_sec-frame_start_time.tv_sec) * NANOS_PER_SECOND
			+ (frame_end_time.tv_nsec-frame_start_time.tv_nsec);
		if (elapsed_nanoseconds < NANOS_PER_FRAME)
			continue;
		else clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
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
	window->window = sdl_win;
	window->renderer = renderer;
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
	SDL_DestroyRenderer(worker->window->renderer);
	SDL_DestroyWindow(worker->window->window);
	SDL_Quit();
	free(worker->window);
	free(worker);
}
