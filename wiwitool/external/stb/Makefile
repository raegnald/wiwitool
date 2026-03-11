.PHONY: all

OBJECTS = stb_image_write.o stb_image.o stb_perlin.o stb_vorbis.o

all: $(OBJECTS)

%.o: %.c
	$(CXX) -c $< -o $@
