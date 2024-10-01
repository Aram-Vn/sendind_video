# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11 `pkg-config --cflags opencv4`

# Libraries to link
LIBS = `pkg-config --libs opencv4`

# Targets
TARGETS = client server

# Object files
CLIENT_OBJ = Client.o
SERVER_OBJ = Server.o

# Build all
all: $(TARGETS)

# Build the client executable
client: $(CLIENT_OBJ)
	$(CXX) -o $@ $^ $(LIBS)

# Build the server executable
server: $(SERVER_OBJ)
	$(CXX) -o $@ $^ $(LIBS)

# Compile the client source file
Client.o: Client.cpp
	$(CXX) $(CXXFLAGS) -c Client.cpp

# Compile the server source file
Server.o: Server.cpp
	$(CXX) $(CXXFLAGS) -c Server.cpp

# Clean up object files and executables
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(TARGETS)
