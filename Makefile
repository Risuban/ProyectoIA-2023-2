# Definir el compilador
CXX=g++

# Definir las banderas de compilación
CXXFLAGS=-std=c++17 -Wall

# Definir el nombre del ejecutable
TARGET=programa

# Definir el archivo fuente
SRC=main.cpp

# La primera regla es la que se ejecuta por defecto ("all").
# Esta regla depende del TARGET, que es el nombre del ejecutable que queremos generar.
all: $(TARGET)

# Regla para generar el ejecutable.
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Regla para limpiar los archivos generados.
clean:
	rm -f $(TARGET)

# Esta es una regla especial que indica que las reglas 'all' y 'clean' no dependen de archivos con esos nombres.
.PHONY: all clean
