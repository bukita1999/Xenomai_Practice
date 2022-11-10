SKIN=alchemy
MAIN_SRC=simple_rt
TARGET=simple_rt

LM=-lm

CFLAGS := $(shell xeno-config --skin=alchemy --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=alchemy --ldflags)
CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
		$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
