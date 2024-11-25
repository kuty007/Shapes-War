CXX := g++
OUTPUT := GAME
OS := $(shell uname)
ifeq ($(OS), Linux)
    CXX_FLAGS := -std=c++17 -Wno-unused-result -Wno-deprecated-declarations
    INCLUDES := -I./src -I./src/imgui
    LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL
endif

SOURCES := $(wildcard src/*.cpp src/imgui/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

#include dependencies files
DEPS := $(SOURCES:.cpp=.d)
-include $(DEPS)
#all of the targets will be compiled if the makefile is run with make
all:  $(OUTPUT) $(OUTPUT)

#define the main executable requirements /command
$(OUTPUT): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o ./bin/$@


#specify how the object files are created
%.o: %.cpp
	$(CXX) -MMD -MP -c $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

#typing make clean will remove all object files
clean:
	rm -f $(OBJECTS) $(DEPS) ./bin/$(OUTPUT)

#typing make run will run the executable
run:
	./bin/$(OUTPUT)

