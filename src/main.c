#include <smoke.h>

worker_data* worker;

void *animation_routine(void *p) {
	/*struct timespec frame_start_time, frame_end_time;
	Uint64 elapsed_nanoseconds;
	memset(&frame_start_time, 0, sizeof(frame_start_time));
	memset(&frame_end_time, 0, sizeof(frame_end_time));*/
	worker_data *worker = (worker_data*)p;
	//clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
	//for(;;) {
		/*pthread_mutex_lock(&worker->halt_mutex);
		if (worker->halt) {
			pthread_mutex_unlock(&worker->halt_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&worker->halt_mutex);
		clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
		elapsed_nanoseconds = (frame_end_time.tv_sec-frame_start_time.tv_sec) * NANOS_PER_SECOND
			+ (frame_end_time.tv_nsec-frame_start_time.tv_nsec);
		if (elapsed_nanoseconds < NANOS_PER_FRAME) return NULL;
		else clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
		pthread_mutex_lock(&worker->data_mutex);*/
		for (Uint32 current_rect = 0; current_rect < worker->window->cur_rects; current_rect++) {
			worker->window->rects[current_rect].x += worker->window->velosX[current_rect];
			worker->window->rects[current_rect].y += worker->window->velosY[current_rect] -Y_FIX_VELOCITY;
			if (fabs(worker->window->velosX[current_rect]) >= VELOCITY_X_INDX)
				worker->window->velosX[current_rect] -= VELOCITY_X_INDX*(worker->window->velosX[current_rect]>0?1:-1);
			else	worker->window->velosX[current_rect] = 0;
			if (fabs(worker->window->velosY[current_rect]) >= VELOCITY_Y_INDX)
				worker->window->velosY[current_rect] -= VELOCITY_Y_INDX*(worker->window->velosY[current_rect]>0?1:-1);
			else	worker->window->velosY[current_rect] = 0;
			if (worker->window->rects[current_rect].w<=RECT_MAX_WIDTH) {
				worker->window->rects[current_rect].w += worker->window->meta[current_rect].scale_x;
				worker->window->rects[current_rect].x -= worker->window->meta[current_rect].scale_x/2;
			}
			if (worker->window->rects[current_rect].h<=RECT_MAX_HEIGHT) {
				worker->window->rects[current_rect].h += worker->window->meta[current_rect].scale_y;
				worker->window->rects[current_rect].y -= worker->window->meta[current_rect].scale_y/2;
			}
			if (worker->window->alphas[current_rect] >= worker->window->meta[current_rect].alpha_indx)
				worker->window->alphas[current_rect] -= worker->window->meta[current_rect].alpha_indx;
			worker->window->meta[current_rect].angle += worker->window->meta[current_rect].indx
				* worker->window->meta[current_rect].dir;
			worker->window->meta[current_rect].angle %= 360;
		}
		//pthread_mutex_unlock(&worker->data_mutex);
	//}
	return NULL;
}

int16_t a_random(int16_t min, int16_t max) { return (rand() % (max - min + 1)) + min; }

void draw_routine(void *p) {
	worker_data *worker = (worker_data*)p;
	struct timespec frame_start_time, frame_end_time;
	Uint64 elapsed_nanoseconds;
	SDL_Event event;
	clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
	//for (;;) {
		animation_routine(p);
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					printf("SDL_QUIT success\n");
					/*pthread_mutex_lock(&worker->halt_mutex);
					worker->halt = 1;
					pthread_mutex_unlock(&worker->halt_mutex);*/
					return;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					worker->window->mouse_active = 0; break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					worker->window->mouse_active = 1;
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if (worker->window->mouse_active) {
						worker->window->meta[worker->window->rIndex].angle = a_random(0, 360);
						worker->window->meta[worker->window->rIndex].dir = a_random(0, 1) ? 1 : -1;
						worker->window->meta[worker->window->rIndex].indx = a_random(1, 2);
						worker->window->meta[worker->window->rIndex].scale_x = a_random(MIN_SCALE_X_INDX, MAX_SCALE_X_INDX);
						worker->window->meta[worker->window->rIndex].scale_y = worker->window->meta[worker->window->rIndex].scale_x;
						worker->window->meta[worker->window->rIndex].alpha_indx = a_random(MIN_ALPHA_INDX, MAX_ALPHA_INDX);
						worker->window->velosX[worker->window->rIndex] = a_random(0, 10);
						worker->window->velosY[worker->window->rIndex] = a_random(0, 10);
						worker->window->alphas[worker->window->rIndex] = a_random(0x64, 0xff);
						worker->window->rects[worker->window->rIndex].w = a_random(DEF_RECT_WIDTH-10,DEF_RECT_WIDTH);
						worker->window->rects[worker->window->rIndex].h = a_random(DEF_RECT_HEIGHT-10,DEF_RECT_HEIGHT);
						worker->window->rects[worker->window->rIndex].x = event.motion.x-worker->window->rects[worker->window->rIndex].w/2;
						worker->window->rects[worker->window->rIndex].y = event.motion.y-worker->window->rects[worker->window->rIndex].h/2;
						//pthread_mutex_lock(&worker->data_mutex);
						worker->window->rIndex++;
						if (worker->window->rIndex >= MAX_RECTS) worker->window->rIndex = 0;
						if (worker->window->cur_rects < MAX_RECTS) worker->window->cur_rects++;
						//pthread_mutex_unlock(&worker->data_mutex);
					}
					break;
				default: break;
			}
		}
		clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
		elapsed_nanoseconds = (frame_end_time.tv_sec-frame_start_time.tv_sec) * NANOS_PER_SECOND
			+ (frame_end_time.tv_nsec-frame_start_time.tv_nsec);
		if (elapsed_nanoseconds < NANOS_PER_FRAME)
			return;
		else clock_gettime(CLOCK_MONOTONIC, &frame_start_time);
		SDL_SetRenderDrawColor(worker->window->renderer,
				(BGC>>24)&0xff,(BGC>>16)&0xff,
				(BGC>>8)&0xff,BGC&0xff);
		SDL_RenderClear(worker->window->renderer);
		//pthread_mutex_lock(&worker->data_mutex);
		for (Uint32 current_rect = 0; current_rect < worker->window->cur_rects; current_rect++) {
			SDL_SetTextureAlphaMod(worker->window->texture, worker->window->alphas[current_rect]);
			SDL_RenderTextureRotated(worker->window->renderer, worker->window->texture,
			NULL, &worker->window->rects[current_rect], worker->window->meta[current_rect].angle, NULL, SDL_FLIP_NONE);
		}
		//pthread_mutex_unlock(&worker->data_mutex);
		SDL_RenderPresent(worker->window->renderer);
	//}
}

bool init_window(worker_data *worker) {
	win *window = worker->window;
	SDL_Window *sdl_win = NULL;
	SDL_Renderer *renderer =  NULL;
	window->win_height = DEF_WIN_HEIGHT;
	window->win_width = DEF_WIN_WIDTH;
	if (!SDL_Init(SDL_INIT_EVENTS))
		return 0;
	sdl_win = SDL_CreateWindow("smoking",
			window->win_width,
			window->win_height, 0);
	renderer = SDL_CreateRenderer(sdl_win, NULL);
	if (!sdl_win || !renderer) return 0;
	SDL_SetWindowMinimumSize(sdl_win,
			MIN_WIN_WIDTH,
			MIN_WIN_HEIGHT);
	SDL_SetRenderDrawBlendMode(worker->window->renderer, SDL_BLENDMODE_BLEND);
	window->window = sdl_win;
	window->renderer = renderer;
	if (!load_texture(worker))
		exit(1);
	return 1;
}

int main() {
	srand(time(NULL));
	worker = (worker_data*)malloc(sizeof(worker_data));
	win *window = (win*)malloc(sizeof(win));
	if (!worker || !window) exit(1);
	memset(worker, 0, sizeof(worker_data));
	memset(window, 0, sizeof(win));
	worker->window = window;
	if (!init_window(worker)) exit(1);
	/*pthread_mutex_init(&worker->halt_mutex, NULL);
	pthread_mutex_init(&worker->data_mutex, NULL);
	pthread_mutex_init(&worker->indx_mutex, NULL);*/
	//pthread_create(&worker->worker, NULL, animation_routine, worker);
	emscripten_set_main_loop_arg(draw_routine, worker, 0, 1);
	//draw_routine(worker);
	/*pthread_join(worker->worker, NULL);
	pthread_mutex_destroy(&worker->halt_mutex);
	pthread_mutex_destroy(&worker->data_mutex);
	pthread_mutex_destroy(&worker->indx_mutex);*/
	SDL_DestroyTexture(worker->window->texture);
	SDL_DestroyRenderer(worker->window->renderer);
	SDL_DestroyWindow(worker->window->window);
	SDL_Quit();
	free(worker->window);
	free(worker);
}
