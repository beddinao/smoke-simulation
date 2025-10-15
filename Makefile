all: 
	@mkdir -p build && cd build && emcmake cmake -DALLOW_MEMORY_GROWTH=1 .. && cmake -DALLOW_MEMORY_GROWTH=1 --build .

clean:
	rm -fr build
