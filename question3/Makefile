CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC1 = question3.cpp

OBJ1 = question3.o


DEBUG_BIN1 = debug_oopd_project_q3

RELEASE_BIN1 = release_oopd_project_q3


.PHONY: all debug release clean

all: debug release

debug: $(OBJ1)
	$(CXX) $(CXXFLAGS) -g -o $(DEBUG_BIN1) $(OBJ1)
	

release: $(OBJ1)
	$(CXX) $(CXXFLAGS) -O2 -o $(RELEASE_BIN1) $(OBJ1)
	

$(OBJ1): $(SRC1)
	$(CXX) $(CXXFLAGS) -c $(SRC1)



clean:
	rm -f $(OBJ1) $(DEBUG_BIN1) $(RELEASE_BIN1) 