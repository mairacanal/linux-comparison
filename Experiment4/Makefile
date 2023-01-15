SKIN=alchemy
MAIN_SRC=experiment4
TARGET=experiment4

LM=-lm

CFLAGS := $(shell /usr/xenomai/bin/xeno-config --skin=alchemy --cflags)
LDFLAGS := $(LM) $(shell /usr/xenomai/bin/xeno-config --skin=alchemy --ldflags) -lgpiod
CC := $(shell /usr/xenomai/bin/xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
		$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
