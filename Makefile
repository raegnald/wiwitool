.PHONY: all desktop wasm front clean run-dev run-desktop run-dev-front

export DEBUG_FLAG ?= -DWIWIDEBUG
export BUILD_FLAGS ?= -g

all: desktop wasm front

wasm:
	$(MAKE) -C wiwitool wasm

	mkdir -p front/public/wasm front/src/bindings/wasm
	cp wiwitool/build/wasm/wiwitool.js front/public/wasm/
	cp wiwitool/build/wasm/wiwitool.wasm front/public/wasm/
	cp wiwitool/build/wasm/wiwitool.data front/public/wasm/
	cp wiwitool/build/wasm/*.ts front/src/bindings/wasm/

desktop:
	$(MAKE) -C wiwitool desktop

front: wasm
	cd front && npm run build

run-dev: wasm
	cd front && npm run dev

run-desktop: desktop
	./wiwitool/build/wiwitool_desktop

clean:
	$(MAKE) -C wiwitool clean

release: export DEBUG_FLAG =
release: export BUILD_FLAGS = -O3
release: clean wasm front desktop
