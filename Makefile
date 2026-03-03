TARGET = Zipcord.exe
CC = gcc

CFLAGS = -Wall -std=c17 -O2 -mwindows
SRC = main.c
LIBS = -lgdi32 -luser32 -lshell32 -lmsimg32
all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	@if exist $(TARGET) del $(TARGET)

.PHONY: all run clean

