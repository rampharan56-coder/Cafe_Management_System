CC       = gcc
CFLAGS   = -Wall -Wextra -pedantic -std=c11 -Iinclude
SRCDIR   = src
SRCS     = $(wildcard $(SRCDIR)/*.c)

# Detect platform for executable extension
ifeq ($(OS),Windows_NT)
TARGET = cafe.exe
RM     = del /Q
else
TARGET = cafe
RM     = rm -f
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
	rm -rf build
	rm -f *.o *.obj

run: $(TARGET)
	./$(TARGET)
