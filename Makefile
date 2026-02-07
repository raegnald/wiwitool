.PHONY: all

all: library

library:
	$(MAKE) -C wiwitool wasm
	@mkdir -p front/public/wasm/
	cp wiwitool/build/wasm/wiwitool.{js,wasm,data} front/public/wasm/

run-dev: library
	cd front && npm run dev
