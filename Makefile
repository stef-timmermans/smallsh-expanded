# Modify the line below to `CC = gcc` for use on Windows
CC = clang
CFLAGS = -Werror -Wall
DEBUGFLAGS = -g
TARGET = smallsh
SOURCES = main.c built_in.c

all: $(TARGET)

debug: CFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
