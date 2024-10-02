# Compiler and flags
CXX       := g++
CXXFLAGS  := -Wall -O2 -std=c++17
LDFLAGS   := -levdev                # Link against libevdev

# Source files
SRC_JOY_SOC     := joy_soc.cpp
SRC_TSP_RESIVER := tsp_resiver.cpp

# Output binaries
BIN_JOY_SOC     := joy_soc
BIN_TSP_RESIVER := tsp_resiver

# Default target (build both programs)
all: $(BIN_JOY_SOC) $(BIN_TSP_RESIVER)

# Rule to build joy_soc
$(BIN_JOY_SOC): $(SRC_JOY_SOC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

# Rule to build tsp_resiver
$(BIN_TSP_RESIVER): $(SRC_TSP_RESIVER)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

# Clean target to remove compiled binaries
clean:
	rm -f $(BIN_JOY_SOC) $(BIN_TSP_RESIVER)

.PHONY: all clean
