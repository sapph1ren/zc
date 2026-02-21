# Путь к vcpkg (можно задать через переменную окружения)
VCPKG_ROOT ?= C:/Users/sapph1ren/vcpkg
# Целевой триплет (например, x64-linux или x64-windows)
TRIPLET ?= x64-mingw-static
# linux
# x64-linux
# Пути vcpkg
VCPKG_PATH = $(VCPKG_ROOT)/installed/$(TRIPLET)
VCPKG_INC  = -I$(VCPKG_PATH)/include
VCPKG_LIB  = $(VCPKG_PATH)/lib

# Настройки компилятора
CC      = gcc
CFLAGS  = -Wall -O2 $(VCPKG_INC) -I./src -DPLATFORM_DESKTOP

# Порядок критически важен для MinGW!
LDFLAGS = -L$(VCPKG_LIB)  -lraylib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32 -loleaut32 -luuid -lwinspool -lshell32 -ladvapi32 -lodbc32 -lodbccp32 -lpthread -static-libgcc -static-libstdc++ -static
# linux
# LDFLAGS = $(VCPKG_LIB) -lraylib -lglfw3 -lGL -lm -lpthread -ldl -lrt -lX11


# Файлы проекта
SOURCES = simple.c microui.c murl.c
HEADERS = murl.h microui.h
# Если microui лежит в другой папке, укажите путь: src/vendor/microui.c
TARGET  = Zipcord

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)


clean:
	rm -f $(TARGET)

