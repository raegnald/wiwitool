.PHONY: all

all: library

library:
	$(MAKE) -C wiwitool wasm
	@mkdir -p front/wasm/
	cp wiwitool/build/wasm/wiwitool.{js,data,wasm} front/wasm/

run-dev: library
	cd front && npm run dev
