# Makefile

# Compiler settings
CC=gcc
CXX=g++
INCLUDES=-I. -Iinclude/ -Ilame/
CFLAGS=-DHAVE_CONFIG_H -DHAVE_MPGLIB
CXXFLAGS=

# Dependencies to install
DEPS=libmp3lame-dev libsndfile1-dev libfftw3-dev libsamplerate0-dev libreadline-dev portaudio19-dev

# Source files
CPP_SOURCES=$(wildcard *.cpp) $(wildcard _func/*.cpp) $(wildcard lame/*.cpp) $(wildcard modules/sys/*.cpp)
C_SOURCES=$(wildcard iir/*.c) $(wildcard *.c) $(wildcard lame/*.c)

# Object files
OBJECTS=$(CPP_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)

# Target executable
TARGET=aux2

.PHONY: all clean install_deps

all: $(TARGET)

# Download json handling file from github
download_nlohmann:
	@mkdir -p include/nlohmann
	wget https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp -O include/nlohmann/json.hpp

# Install dependencies
install_deps:
	sudo apt update
	sudo apt install -y $(DEPS)

# Target build
$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ -lsndfile -lreadline -lmp3lame -lportaudio -lfftw3 -lsamplerate -lpthread

# Compile C++ sources
%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

# Compile C sources
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -f $(OBJECTS)

