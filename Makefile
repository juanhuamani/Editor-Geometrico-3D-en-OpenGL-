# Makefile - Computacion Grafica - Laboratorio 7
# Percy Maldonado Quispe

CXX      = g++
CXXFLAGS = -Wall -std=c++11
LIBS     = -lGL -lGLU -lglut
TARGET   = editor3d
SRC      = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
