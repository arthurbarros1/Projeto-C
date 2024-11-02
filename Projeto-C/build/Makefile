
EXECUTABLE := main


BUILD_DIR := ../bin


CC := gcc
CFLAGS := -Wall -g -I../include


SOURCES := ../src/main.c ../src/keyboard.c ../src/screen.c ../src/timer.c


all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(EXECUTABLE) $(SOURCES)


clean:
	rm -f $(BUILD_DIR)/$(EXECUTABLE)

.PHONY: all clean
