%.exe: %.cpp

painting_tool.exe: ../build/native/libwiwi.a painting_tool.cpp
	$(CXX) -I../include -std=c++23 -g ../build/native/libwiwi.a painting_tool.cpp -o painting_tool.exe
