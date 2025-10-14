all: 
	@mkdir -p build && cd build && emcmake cmake .. && emmake make -j20

clean:
	rm -fr build
