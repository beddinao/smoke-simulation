all:
	emcmake cmake -B build -DCMAKE_EXE_LINKER_FLAGS="-sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -sEXPORTED_FUNCTIONS=_load_texture_web,_draw_routine,_main"
	cmake --build build
