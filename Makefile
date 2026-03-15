.PHONY: all

export DEBUG_FLAG ?= -DWIWIDEBUG
export BUILD_FLAGS ?= -g

all: front-dependencies

library:
	$(MAKE) -C wiwitool wasm

clean-library:
	$(MAKE) -C wiwitool clean

front-dependencies: library
	mkdir -p front/public/wasm front/src/bindings
	cp wiwitool/build/wasm/wiwitool.{js,wasm,data} front/public/wasm/
	cp wiwitool/build/wasm/*.ts front/src/bindings/

# Debug

run-dev: front-dependencies
	cd front && npm run dev

# Release

release: export DEBUG_FLAG =
release: export BUILD_FLAGS = -O3
release:
	$(MAKE) clean-library
	$(MAKE) front-dependencies
	cd front && npm run build

run-release: release
	cd front/dist/ && python3 -m http.server 8080
