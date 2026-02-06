.PHONY: all clean

SRCS := $(wildcard *.cpp)
EXES := $(SRCS:.cpp=.exe)

all: $(EXES)

%.exe: %.cpp ../build/native/libwiwi.a
	$(CXX) -I../include -std=c++23 -g ../build/native/libwiwi.a $< -o $@

clean:
	rm -rf $(EXES) datapack/ respack/
