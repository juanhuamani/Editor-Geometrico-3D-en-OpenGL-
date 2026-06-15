# Makefile - Computacion Grafica - Laboratorio 7
# Percy Maldonado Quispe

CXX      = g++
CXXFLAGS = -Wall -std=c++11
LIBS     = -lGL -lGLU -lglut
TARGET   = editor3d

SRCDIR   = src
SRC      = $(wildcard $(SRCDIR)/*.cpp)
OBJ      = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(OBJ) $(LIBS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(SRCDIR)/*.o

.PHONY: all run clean