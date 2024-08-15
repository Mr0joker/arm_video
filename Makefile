CC = arm-linux-gcc
CFLAGS = -L./libjpeg  -I./libjpeg  -ljpeg -pthread -lsqlite3 -L/home/china/arm_libs/lib -I/home/china/arm_libs/include

TARGET = main

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(wildcard *.h)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)