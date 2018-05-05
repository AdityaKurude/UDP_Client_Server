TARGET = server
LIBS = -lsqlite3
CC = gcc
CFLAGS = -g

.PHONY: default all clean

default: $(TARGET)
all: default

debug: DEBUG = -D DEBUG
debug: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(DEBUG) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o *.db
	-rm -f $(TARGET)
