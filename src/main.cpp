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
		/*for (Uint32 current_rect = 0; current_rect < worker->window->rIndex; current_rect) {
			if (worker->window->alphas[current_rect] > ALPHA_INDX)
				worker->window->alphas[current_rect] -= ALPHA_INDX;
		}*/
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
					std::cout << "SDL_QUIT success" << std::endl;
					pthread_mutex_lock(&worker->halt_mutex);
					worker->halt = true;
					pthread_mutex_unlock(&worker->halt_mutex);
					return;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					worker->window->mouse_active = false; break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					worker->window->mouse_iter = 0;
					worker->window->mouse_active = true;
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if (worker->window->mouse_active) {
						worker->window->mouse_iter += 1;
						if (worker->window->mouse_iter >= 10)
							worker->window->mouse_iter = 0;
						else break;
						worker->window->velosX[worker->window->rIndex] = event.motion.xrel;
						worker->window->velosY[worker->window->rIndex] = event.motion.yrel;
						worker->window->alphas[worker->window->rIndex] = (rand() % 0xff) + 0xf0;
						worker->window->rects[worker->window->rIndex].x = event.motion.x-DEF_RECT_WIDTH/2;
						worker->window->rects[worker->window->rIndex].y = event.motion.y-DEF_RECT_HEIGHT/2;
						worker->window->rects[worker->window->rIndex].w = DEF_RECT_WIDTH;
						worker->window->rects[worker->window->rIndex++].h = DEF_RECT_HEIGHT;
						if (worker->window->rIndex >= MAX_RECTS) worker->window->rIndex = 0;
					}
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
		for (Uint32 current_rect = 0; current_rect < worker->window->rIndex; current_rect++) {
			worker->window->rects[current_rect].x += worker->window->velosX[current_rect];
			worker->window->rects[current_rect].y += worker->window->velosY[current_rect] -1;
			if (abs(worker->window->velosX[current_rect]) >= VELOCITY_X_INDX)
				worker->window->velosX[current_rect] -=
					VELOCITY_X_INDX*(worker->window->velosX[current_rect]>0?1:-1);
			else worker->window->velosX[current_rect] = 0;
			if (abs(worker->window->velosY[current_rect]) >= VELOCITY_Y_INDX)
				worker->window->velosY[current_rect] -=
					VELOCITY_Y_INDX*(worker->window->velosY[current_rect]>0?1:-1);
			else worker->window->velosY[current_rect] = 0;
			if (worker->window->rects[current_rect].w<=RECT_MAX_WIDTH)
				worker->window->rects[current_rect].w += SCALE_W_INDX;
			if (worker->window->rects[current_rect].h<=RECT_MAX_HEIGHT)
				worker->window->rects[current_rect].h += SCALE_H_INDX;
			SDL_SetTextureAlphaMod(worker->window->texture, worker->window->alphas[current_rect]);
			if (worker->window->alphas[current_rect] >= ALPHA_INDX)
				worker->window->alphas[current_rect] -= ALPHA_INDX;
			SDL_RenderTexture(worker->window->renderer, worker->window->texture,
					NULL, &worker->window->rects[current_rect]);
		}
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
	SDL_SetRenderDrawBlendMode(worker->window->renderer, SDL_BLENDMODE_BLEND);	
	SDL_Surface *surface = IMG_Load(TEXTURE_PATH);
	if (!surface || !(worker->window->texture = SDL_CreateTextureFromSurface(
			renderer, surface))) exit(1);
	SDL_SetTextureBlendMode(worker->window->texture, SDL_BLENDMODE_BLEND);
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
