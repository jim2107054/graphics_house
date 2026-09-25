CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Isrc
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

SRC = src/main.cpp src/glad.c src/stb_image.cpp
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	del /f /q $(TARGET) 2>nul || rm -f $(TARGET)
