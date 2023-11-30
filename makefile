all:
	cc assets/ptf.c -lm -Ofast -o assets/ptf
	mkdir -p release
	cc main.c glad_gl.c -I inc -Ofast -lglfw -lm -o release/tuxocide
	strip --strip-unneeded release/tuxocide
	upx --lzma --best release/tuxocide

test:
	cc main.c glad_gl.c -I inc -Ofast -lglfw -lm -o /tmp/tuxocide_test
	/tmp/tuxocide_test
	rm /tmp/tuxocide_test

web:
	emcc main.c glad_gl.c -DWEB -O3 --closure 1 -s FILESYSTEM=0 -s USE_GLFW=3 -s ENVIRONMENT=web -s TOTAL_MEMORY=128MB -I inc -o bin/index.html --shell-file t.html

run:
	emrun bin/index.html

clean:
	rm -f -r release
	rm -f assets/ptf
	rm -f bin/index.html
	rm -f bin/index.js
	rm -f bin/index.wasm
