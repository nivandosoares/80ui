CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?= -lncurses

TARGET = retro_ui
SRC = main.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
