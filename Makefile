CXX = g++

compile:
	$(CXX) main.cpp -o ./build/linux.out -std=c++0x -O3

run: compile
	"./build/linux.out"


