# --- Configuration ---

# Compilers
CXX_NATIVE := clang++
CXX_WASM   := em++
AR_NATIVE  := ar
AR_WASM    := emar

DEBUG_FLAG ?= -DWIWIDEBUG
BUILD_FLAGS ?= -g

# Flags common to all build targets
CPPFLAGS := -Iinclude -Iexternal/json/include -Iexternal/stb -Iexternal/miniz_cpp $(DEBUG_FLAG)
CXXFLAGS := -Wall -std=c++23 $(BUILD_FLAGS) $(CPPFLAGS)

# Native-specific flags
# Note: vorbisenc is needed specifically for vorbis_encode_init_vbr
NATIVE_PKG_FLAGS := $(shell pkg-config --cflags vorbisenc vorbis ogg)
CXXFLAGS_NATIVE  := $(CXXFLAGS) $(NATIVE_PKG_FLAGS)

# WASM-specific flags (emscripten)
WASM_PORT_FLAGS := -s USE_VORBIS=1 -s USE_OGG=1
CXXFLAGS_WASM   := $(CXXFLAGS) $(WASM_PORT_FLAGS) -O3 -flto

# Directories
SRC_DIR    := implementation
WASM_SRC   := wasm
ASSETS_DIR := assets/
BUILD_DIR  := build

# Source files
SRCS       := $(shell find $(SRC_DIR) -type file -name '*.cpp')

# External object files
EXT_OBJS   := external/json/json.o            \
              external/stb/stb_image.o        \
              external/stb/stb_image_write.o  \
              external/stb/stb_perlin.o       \
              external/stb/stb_vorbis.o

# --- Targets ---

.PHONY: all native wasm clean flags

all: native wasm

# Create build directories
$(BUILD_DIR)/native:
	mkdir -p $@

$(BUILD_DIR)/wasm:
	mkdir -p $@

# ==========================================
# FLAGS GENERATION (compile_flags.txt)
# ==========================================

flags: compile_flags.txt

compile_flags.txt: Makefile
	@echo "--- Generating compile_flags.txt ---"
	@# We use the NATIVE flags here so clangd can find the libvorbis headers on the host machine
	@echo "$(CXXFLAGS_NATIVE)" | tr ' ' '\n' > compile_flags.txt

# ==========================================
# NATIVE BUILD (Static Library .a)
# ==========================================

# Location for native objects
NATIVE_OBJ_DIR := $(BUILD_DIR)/native
NATIVE_OBJS    := $(patsubst $(SRC_DIR)/%.cpp, $(NATIVE_OBJ_DIR)/%.o, $(SRCS))
NATIVE_TARGET  := $(NATIVE_OBJ_DIR)/libwiwi.a

# Added 'compile_flags.txt' as a dependency here
native: $(BUILD_DIR)/native compile_flags.txt
	@echo "--- Building Native ---"
	$(MAKE) -C external CXX=$(CXX_NATIVE) AR=$(AR_NATIVE)
	$(MAKE) $(NATIVE_TARGET)

# Compile native objects
$(NATIVE_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_NATIVE) $(CXXFLAGS_NATIVE) -c $< -o $@

# Rule to create the static library archive
$(NATIVE_TARGET): $(NATIVE_OBJS)
	$(AR_NATIVE) rcs $@ $(NATIVE_OBJS) $(EXT_OBJS)
	@echo "Native lib created at: $@"

# ==========================================
# WASM BUILD (Module .js/.wasm)
# ==========================================

# Location for wasm objects
WASM_OBJ_DIR := $(BUILD_DIR)/wasm
WASM_OBJS    := $(patsubst $(SRC_DIR)/%.cpp, $(WASM_OBJ_DIR)/%.o, $(SRCS))
WASM_TARGET  := $(WASM_OBJ_DIR)/wiwitool.js

wasm: $(BUILD_DIR)/wasm
	@echo "--- Building WebAssembly ---"
	$(MAKE) -C external CXX=$(CXX_WASM) AR=$(AR_WASM)
	$(MAKE) $(WASM_TARGET)

# Compile WASM objects (from implementation/)
$(WASM_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_WASM) $(CXXFLAGS_WASM) -c $< -o $@

# Link everything into the final WASM module
$(WASM_TARGET): $(WASM_OBJS)
	$(CXX_WASM) $(CXXFLAGS_WASM) \
		$(WASM_OBJS) \
		$(EXT_OBJS) \
		--bind \
		-o $@ \
		-s NO_DISABLE_EXCEPTION_CATCHING \
		-s WASM=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s FORCE_FILESYSTEM=1 \
		-s MODULARIZE=1 \
		-s EXPORT_NAME='createWiwitoolModule' \
		-s EXPORTED_RUNTIME_METHODS='["FS"]' \
		--emit-tsd 'wiwitool.d.ts' \
		--preload-file $(ASSETS_DIR)@/
	@echo "WASM module created at: $@"

# --- Cleaning ---

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C external clean
