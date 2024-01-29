CPP_FILES := $(wildcard source/*.cpp)
OBJ_FILES := $(addprefix output/,$(notdir $(CPP_FILES:.cpp=.o)))

#LIB_FILES := jsoncpp
LD_FLAGS := -lGLEW -lGL -lSDL2 -lSOIL
CC_FLAGS := -Wextra -Wall -ansi -pedantic -std=c++11 -g

# Link object files
main.exe: $(OBJ_FILES)
	g++ -o $@ $^ $(LD_FLAGS)

# Make object files for all source files
output/%.o: source/%.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	-rm output/*.o

all:
	make clean
	make
