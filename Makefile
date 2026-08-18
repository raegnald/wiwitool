.PHONY: all desktop wasm front clean run-dev run-desktop run-dev-front release

export DEBUG_FLAG ?= -DWIWIDEBUG
export BUILD_FLAGS ?= -g

CMAKE_BUILD_TYPE = $(if $(DEBUG_FLAG),Debug,Release)

WASM_DIR := wiwitool/build/wasm
NATIVE_DIR := wiwitool/build/native

WASM_BASE_FLAGS := $(BUILD_FLAGS) -fexceptions -O3 -flto -s USE_VORBIS=1 -s USE_OGG=1 -DEMSCRIPTEN $(DEBUG_FLAG)

# -include emscripten/bind.h forces the compiler to parse Emscripten's templates
# cleanly BEFORE stb_vorbis.h defines macros 'C' and 'R' that collide with other
# parts of the code.
WASM_CXXFLAGS := $(WASM_BASE_FLAGS) --bind -include emscripten/bind.h

WASM_CFLAGS := $(WASM_BASE_FLAGS)

all: desktop wasm front

wasm:
	@echo "*** Compiling WASM with CMake"

	@mkdir -p $(WASM_DIR)
	emcmake cmake -S wiwitool -B $(WASM_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DCMAKE_CXX_FLAGS="$(WASM_CXXFLAGS)" \
		-DCMAKE_C_FLAGS="$(WASM_CFLAGS)"
	cmake --build $(WASM_DIR) --target wiwi -j

	@echo "*** Linking WASM with em++"

	# We use find to link the raw .o files directly. This bypasses the bug where LTO
	# aggressively strips Embind and STB symbols out of static .a archives.
	em++ $(WASM_BASE_FLAGS) --bind \
		`find $(WASM_DIR)/CMakeFiles/wiwi.dir -name "*.o"` \
		`find $(WASM_DIR)/external/json/CMakeFiles/json.dir -name "*.o"` \
		`find $(WASM_DIR)/external/stb/CMakeFiles/stb.dir -name "*.o"` \
		-o $(WASM_DIR)/wiwitool.js \
		-s DISABLE_EXCEPTION_CATCHING=0 \
		-s WASM=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s MAXIMUM_MEMORY=4GB \
		-s FORCE_FILESYSTEM=1 \
		-s MODULARIZE=1 \
		-s EXPORT_NAME='createWiwitoolModule' \
		-s EXPORTED_RUNTIME_METHODS='["FS", "getExceptionMessage"]' \
		--preload-file wiwitool/assets@/

	@echo "*** Copying build artifacts to front-end"

	@mkdir -p front/public/wasm front/src/bindings/wasm
	cp $(WASM_DIR)/wiwitool.js front/public/wasm/
	cp $(WASM_DIR)/wiwitool.wasm front/public/wasm/
	cp $(WASM_DIR)/wiwitool.data front/public/wasm/
	cp $(WASM_DIR)/*.ts front/src/bindings/wasm/ 2>/dev/null || true

desktop:
	@echo "*** Building desktop application"

	@mkdir -p $(NATIVE_DIR)
	cmake -S wiwitool -B $(NATIVE_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	cmake --build $(NATIVE_DIR) --target wiwidesktop -j

front: wasm
	cd front && npm run build

run-dev:
	$(MAKE) wasm
	cd front && npm run dev

run-desktop: desktop
	./$(NATIVE_DIR)/wiwidesktop

clean:
	rm -rf wiwitool/build
	rm -rf front/public/wasm/wiwitool.*

release: export DEBUG_FLAG =
release: export BUILD_FLAGS = -O3
release: export CMAKE_BUILD_TYPE = Release
release: clean wasm front desktop
