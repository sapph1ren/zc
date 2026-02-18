# Makefile для сборки microui + fenster + PR #78 (с поддержкой изображений)

# Основные цели
.PHONY: all clean run

# Имя исполняемого файла
TARGET = Zipcord

# Исходные файлы
SOURCES = main.c microui.c renderer.c
# Если у тебя рендерер вынесен в отдельный файл:
# SOURCES = main.c microui.c renderer.c

# Заголовки (single-header, не компилируем отдельно)
HEADERS = fenster.h stb_image.h microui.h renderer.h
# Если renderer отдельно:
# HEADERS += renderer.h

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Wl,-subsystem,windows

# Платформо-зависимые библиотеки
# Linux/X11
#LDFLAGS += -lX11 -ldl

# Windows (MinGW) — раскомментируй, если на Windows
LDFLAGS += -lgdi32 -lopengl32

# macOS (раскомментируй, если нужно)
# LDFLAGS += -framework Cocoa -framework OpenGL

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

# Запуск (удобно)
run: $(TARGET)
	./$(TARGET)

# Очистка
clean:
	rm -f $(TARGET) *.o

# Зависимости (если добавишь renderer.c)
# renderer.o: renderer.c renderer.h microui.h fenster.h
