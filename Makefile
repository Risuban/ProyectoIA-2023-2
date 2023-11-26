# Makefile

# Define el compilador
CC=gcc

# Define las opciones de compilación
CFLAGS=-Wall -std=c++11 -lstdc++

# Define el nombre del ejecutable
TARGET=mi_programa

all: $(TARGET)

$(TARGET): main.cpp
	$(CC) main.cpp $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
