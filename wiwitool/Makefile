# --- Configuration ---

# Compilers
CXX_NATIVE := clang++
CXX_WASM   := em++
AR_NATIVE  := ar
AR_WASM    := emar

# Flags common to all build targets
CPPFLAGS := -Iinclude -Iexternal/json/include -Iexternal/stb -Iexternal/zip_file
CXXFLAGS := -Wall -std=c++23 -g $(CPPFLAGS)

# Directories
SRC_DIR   := implementation
WASM_SRC  := wasm
BUILD_DIR := build

# Source files
SRCS      := $(wildcard $(SRC_DIR)/*.cpp)

# External object files
EXT_OBJS  := external/json/json.o            \
             external/stb/stb_image.o        \
             external/stb/stb_image_write.o  \
             external/zip_file/zip_file.o

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

# We force this to run to ensure the file is always up to date with the Makefile
flags: compile_flags.txt

compile_flags.txt: Makefile
	@echo "--- Generating compile_flags.txt ---"
	@# We use tr to replace spaces with newlines because clangd expects one flag per line
	@echo "$(CXXFLAGS)" | tr ' ' '\n' > compile_flags.txt

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
	$(CXX_NATIVE) $(CXXFLAGS) -c $< -o $@

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
	$(CXX_WASM) $(CXXFLAGS) -c $< -o $@

# Link everything into the final WASM module
$(WASM_TARGET): $(WASM_OBJS) $(WASM_SRC)/executable.cpp
	$(CXX_WASM) $(CXXFLAGS) \
		$(WASM_SRC)/executable.cpp \
		$(WASM_OBJS) \
		$(EXT_OBJS) \
    --bind \
		-o $@ \
		-s WASM=1 -s ALLOW_MEMORY_GROWTH=1
	@echo "WASM module created at: $@"

# --- Cleaning ---

clean:
	rm -rf $(BUILD_DIR)
	rm -f compile_flags.txt
	$(MAKE) -C external clean
