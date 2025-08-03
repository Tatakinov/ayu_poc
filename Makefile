CXXFLAGS=-g -Wall -std=c++20 -I . -I include $(shell pkg-config --cflags glfw3) $(shell pkg-config --cflags glew) $(shell pkg-config --cflags glm)
LDFLAGS=$(shell pkg-config --libs glfw3) $(shell pkg-config --libs glew) $(shell pkg-config --libs glm)
OBJ=$(shell find -name "*.cc" | sed -e 's/\.cc$$/.o/g')
TARGET=ayu_poc.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(OBJ)
