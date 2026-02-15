.PHONY: all

all: front-dependencies

library:
	$(MAKE) -C wiwitool wasm

front-dependencies: library
	mkdir -p front/public/wasm
	cp wiwitool/build/wasm/wiwitool.{js,wasm,data} front/public/wasm/
	cp wiwitool/build/wasm/*.ts front/src/bindings/

run-dev: front-dependencies
	cd front && npm run dev
