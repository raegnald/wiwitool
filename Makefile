.PHONY: all

all: library

library:
	$(MAKE) -C wiwitool wasm
	cp wiwitool/build/wasm/wiwitool.{js,wasm} front/wasm

run: library
	cd front && python3 -m http.server 8080
