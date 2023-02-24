# Name of the project
PROJ_NAME=cmd8
 
# .c files
C_SOURCE=$(wildcard ./src/*.cpp)
 
# .h files
H_SOURCE=$(wildcard ./src/*.hpp)
 
# Object files
OBJ=$(C_SOURCE:.c=.o)
 
# Compiler
CC=g++
 
# Flags for compiler
CC_FLAGS=-O3 -static -static-libgcc -static-libstdc++
 
#
# Compilation and linking
#
all: $(PROJ_NAME)
 
$(PROJ_NAME): $(OBJ)
	$(CC) -o $@ $^
 
%.o: %.cpp %.hpp
	$(CC) -o $@ $< $(CC_FLAGS)
 
main.o: main.cpp $(H_SOURCE)
	$(CC) -o $@ $< $(CC_FLAGS)
 
clean:
	rm -rf *.o $(PROJ_NAME) *~