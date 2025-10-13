all: 
	rm -rf smoke
	@cmake -B build
	@cmake --build build
	@mv build/smoke .

clean:
	rm -fr build smoke
