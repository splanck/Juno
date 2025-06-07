CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -Iinclude

SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:.cpp=.o)

compiler: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f src/*.o compiler
