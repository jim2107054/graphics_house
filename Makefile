CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm

SRC = src/main.cpp \
      src/stb_image.cpp \
      src/core/Config.cpp \
      src/core/Camera.cpp \
      src/core/Input.cpp \
      src/audio/AudioSystem.cpp \
      src/graphics/TextureManager.cpp \
      src/graphics/Primitives.cpp \
      src/graphics/Lighting.cpp \
      src/graphics/Shadow.cpp \
      src/entities/SkyAndStars.cpp \
      src/entities/Terrain.cpp \
      src/entities/Vegetation.cpp \
      src/entities/Graveyard.cpp \
      src/entities/Props.cpp \
      src/entities/Particles.cpp \
      src/entities/House.cpp \
      src/ui/HUD.cpp \
      src/ui/Screenshot.cpp
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	del /f /q $(TARGET) 2>nul || rm -f $(TARGET)
