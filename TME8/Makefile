# Compiler et options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

# Répertoires
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Fichiers sources et objets
SRCS = $(wildcard $(SRC_DIR)/*.cpp) src/ServerSocket.cpp src/Socket.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Nom des exécutables
SERVER_EXEC = $(BIN_DIR)/server
SERVER2_EXEC = $(BIN_DIR)/server2
CLIENT_EXEC = $(BIN_DIR)/client

# Règle par défaut
all: $(SERVER_EXEC) $(SERVER2_EXEC) $(CLIENT_EXEC)

# Compilation des exécutables
$(SERVER_EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(filter %server.o, $(OBJS)) -o $@

$(SERVER2_EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(filter %server2.o, $(OBJS)) -o $@

$(CLIENT_EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(filter %client.o, $(OBJS)) -o $@

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Création des répertoires
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean