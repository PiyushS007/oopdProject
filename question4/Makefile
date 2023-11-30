CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC1 = question4_1.cpp
SRC2 = question4_2.cpp
OBJ1 = question4_1.o
OBJ2 = question4_2.o

DEBUG_BIN1 = debug_oopd_project_q4_1
DEBUG_BIN2 = debug_oopd_project_q4_2

RELEASE_BIN1 = release_oopd_project_q4_1
RELEASE_BIN2 = release_oopd_project_q4_2

.PHONY: all debug release clean

all: debug release

debug: $(OBJ1) $(OBJ2)
	$(CXX) $(CXXFLAGS) -g -o $(DEBUG_BIN1) $(OBJ1)
	$(CXX) $(CXXFLAGS) -g -o $(DEBUG_BIN2) $(OBJ2)

release: $(OBJ1) $(OBJ2)
	$(CXX) $(CXXFLAGS) -O2 -o $(RELEASE_BIN1) $(OBJ1)
	$(CXX) $(CXXFLAGS) -O2 -o $(RELEASE_BIN2) $(OBJ2)

$(OBJ1): $(SRC1)
	$(CXX) $(CXXFLAGS) -c $(SRC1) -o $(OBJ1)

$(OBJ2): $(SRC2)
	$(CXX) $(CXXFLAGS) -c $(SRC2) -o $(OBJ2)

clean:
	rm -f $(OBJ1) $(DEBUG_BIN1) $(RELEASE_BIN1) $(OBJ2) $(DEBUG_BIN2) $(RELEASE_BIN2)
