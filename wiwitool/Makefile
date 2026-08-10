# --- Configuration ---

CXX_NATIVE := clang++
AR_NATIVE  := ar
CXX_WASM   := em++
AR_WASM    := emar

DEBUG_FLAG ?= -DWIWIDEBUG
BUILD_FLAGS ?= -g

SRC_DIR    := implementation
ASSETS_DIR := assets/
BUILD_DIR  := build

SAUCER_BUILD_DIR := $(BUILD_DIR)/saucer
SAUCER_LIB       := $(SAUCER_BUILD_DIR)/libsaucer.a

SAUCER_INCLUDES  := -Iexternal/saucer/include \
                    -Iexternal/saucer/private \
                    -Iexternal/saucer/private/saucer \
                    -I$(SAUCER_BUILD_DIR)/_deps/glaze-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/saucer-fill-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/functional-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/saucer-embed-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/ereignis-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/coco-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/flagpp-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/lockpp-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/rebind-src/include \
                    -I$(SAUCER_BUILD_DIR)/_deps/polo-src/include \
                    -DFILL_NAMESPACE=std -DSAUCER_WEBKIT -DSAUCER_WEBKIT_PRIVATE

SAUCER_LIBS      := $(SAUCER_BUILD_DIR)/libsaucer.a \
                    $(SAUCER_BUILD_DIR)/_deps/coco-build/libcoco.a

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  PLATFORM_LDFLAGS := -framework WebKit -framework Cocoa -framework CoreGraphics -framework AppKit
else ifeq ($(UNAME_S),Linux)
  PLATFORM_LDFLAGS := $(shell pkg-config --libs webkit2gtk-4.1 2>/dev/null || pkg-config --libs webkit2gtk-4.0 2>/dev/null)
else
  # Windows
  PLATFORM_LDFLAGS := -lWebView2Loader
endif

NATIVE_PKG_FLAGS := $(shell pkg-config --cflags vorbisenc vorbis ogg)
NATIVE_PKG_LIBS  := $(shell pkg-config --libs vorbisenc vorbis ogg)

CPPFLAGS := -Iinclude -Inative-bridge -Iexternal/json/include -Iexternal/stb -Iexternal/miniz_cpp -Iexternal/pfd $(SAUCER_INCLUDES) $(DEBUG_FLAG)
CXXFLAGS := -Wall -std=c++23 $(BUILD_FLAGS) $(CPPFLAGS) $(NATIVE_PKG_FLAGS)

WASM_PORT_FLAGS := -s USE_VORBIS=1 -s USE_OGG=1 --bind -DEMSCRIPTEN
CXXFLAGS_WASM   := -Wall -std=c++23 $(BUILD_FLAGS) -Iinclude -Iexternal/json/include -Iexternal/stb -Iexternal/miniz_cpp $(DEBUG_FLAG) $(WASM_PORT_FLAGS) -O3 -flto -fexceptions

SRCS       := $(shell find $(SRC_DIR) -type f -name '*.cpp')

EXT_OBJS   := external/json/json.o            \
              external/stb/stb_image.o        \
              external/stb/stb_image_write.o  \
              external/stb/stb_perlin.o       \
              external/stb/stb_vorbis.o

NATIVE_OBJ_DIR := $(BUILD_DIR)/native
NATIVE_OBJS    := $(patsubst $(SRC_DIR)/%.cpp, $(NATIVE_OBJ_DIR)/%.o, $(SRCS))
NATIVE_LIB     := $(NATIVE_OBJ_DIR)/libwiwi.a
DESKTOP_TARGET := $(BUILD_DIR)/wiwitool_desktop

WASM_OBJ_DIR := $(BUILD_DIR)/wasm
WASM_OBJS    := $(patsubst $(SRC_DIR)/%.cpp, $(WASM_OBJ_DIR)/%.o, $(SRCS))
WASM_TARGET  := $(WASM_OBJ_DIR)/wiwitool.js

.PHONY: all native desktop wasm saucer clean flags

all: desktop wasm

$(BUILD_DIR)/native:
	mkdir -p $@

$(BUILD_DIR)/wasm:
	mkdir -p $@

saucer: $(SAUCER_LIB)

$(SAUCER_LIB):
	@echo "--- Configuring & Building Saucer ---"
	mkdir -p $(SAUCER_BUILD_DIR)
	cmake -B $(SAUCER_BUILD_DIR) -S external/saucer -Dsaucer_no_compiler_version_check=ON -Dsaucer_examples=OFF -Dsaucer_tests=OFF
	cmake --build $(SAUCER_BUILD_DIR)

flags: compile_flags.txt

compile_flags.txt: Makefile
	@echo "--- Generating compile_flags.txt ---"
	@echo "$(CXXFLAGS)" | tr ' ' '\n' > compile_flags.txt

native: $(BUILD_DIR)/native compile_flags.txt
	@echo "--- Building Native Library ---"
	$(MAKE) -C external clean
	$(MAKE) -C external CXX=$(CXX_NATIVE) AR=$(AR_NATIVE)
	$(MAKE) $(NATIVE_LIB)

$(NATIVE_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_NATIVE) $(CXXFLAGS) -c $< -o $@

$(NATIVE_LIB): $(NATIVE_OBJS)
	$(AR_NATIVE) rcs $@ $(NATIVE_OBJS) $(EXT_OBJS)
	@echo "Native lib created at: $@"

desktop: native saucer
	@echo "--- Building Native Desktop Application ---"
	$(CXX_NATIVE) $(CXXFLAGS) desktop_main.cpp $(NATIVE_LIB) $(EXT_OBJS) $(SAUCER_LIBS) $(NATIVE_PKG_LIBS) $(PLATFORM_LDFLAGS) -o $(DESKTOP_TARGET)
	@echo "Native Desktop binary created at: $(DESKTOP_TARGET)"

wasm: $(BUILD_DIR)/wasm
	@echo "--- Building WebAssembly ---"
	$(MAKE) -C external clean
	$(MAKE) -C external CXX=$(CXX_WASM) AR=$(AR_WASM)
	$(MAKE) $(WASM_TARGET)
	@mkdir -p ../front/public/wasm
	cp $(WASM_OBJ_DIR)/wiwitool.js $(WASM_OBJ_DIR)/wiwitool.wasm $(WASM_OBJ_DIR)/wiwitool.data ../front/public/wasm/
	@echo "WASM module created and copied to front/public/wasm/"

$(WASM_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_WASM) $(CXXFLAGS_WASM) -c $< -o $@

$(WASM_TARGET): $(WASM_OBJS)
	$(CXX_WASM) $(CXXFLAGS_WASM) \
		$(WASM_OBJS) \
		$(EXT_OBJS) \
		-o $@ \
		-s DISABLE_EXCEPTION_CATCHING=0 \
		-s WASM=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s MAXIMUM_MEMORY=4GB \
		-s FORCE_FILESYSTEM=1 \
		-s MODULARIZE=1 \
		-s EXPORT_NAME='createWiwitoolModule' \
		-s EXPORTED_RUNTIME_METHODS='["FS", "getExceptionMessage"]' \
		--preload-file $(ASSETS_DIR)@/

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C external clean
