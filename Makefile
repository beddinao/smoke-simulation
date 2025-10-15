CXX = emcc
SRC = $(wildcard src/*.c)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
SDL_PATH = ./assets/SDL3
SDL_IMAGE_PATH = ./assets/SDL3_image
CFLAGS = -Iinclude
LDFLAGS = -Llib -Wl,-rpath,lib -Wl,-lSDL3 -lSDL3_image
NAME = smoke.html

all: sdl $(NAME)

sdl:
	@mkdir -p lib
	@cp -r $(SDL_PATH)/include/SDL3 include
	@cp -r $(SDL_PATH)/build/libSDL3* lib
	@cp -r $(SDL_IMAGE_PATH)/include/SDL3_image include
	@cp -r $(SDL_IMAGE_PATH)/build/libSDL3_image* lib

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS) -O3 -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -sEXPORTED_FUNCTIONS=_load_texture_web,_draw_routine,_main

build/%.o: src/%.c $(HR)
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf build

fclean: clean
	rm -rf include/SDL3
	rm -rf include/SDL3_image
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
