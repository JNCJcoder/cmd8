# Name of the project
PROJ_NAME=cmd8
 
# .cpp files
CPP_DIR=src
CPP_SOURCE=$(wildcard $(CPP_DIR)/*.cpp)
 
# Object files
OBJ_DIR=obj
OBJ=$(patsubst $(CPP_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCE))
 
# Compiler
CXX=g++
 
# Flags
CXX_FLAGS=-O3
LD_FLAGS=-flto -static -static-libgcc -static-libstdc++ -lwinmm
 
#
# Compilation and linking
#
all: $(PROJ_NAME)
 
$(PROJ_NAME): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(CPP_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
 
clean:
	rm -rf $(OBJ_DIR) $(PROJ_NAME)

.PHONY: all clean