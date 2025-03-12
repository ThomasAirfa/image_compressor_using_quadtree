CC := gcc

BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src
LIB_DIR := lib

CFLAGS := -Wall -Wextra -O2 -Iinclude -I$(LIB_DIR)
LFLAGS := -Wl,-rpath,$(LIB_DIR) -L$(LIB_DIR)

EXEC := $(BIN_DIR)/codec
MAIN_C := $(SRC_DIR)/main.c
MAIN_O := $(BIN_DIR)/main.o
LIBRARY := $(LIB_DIR)/libqtc.so

all: 
	make -f Makelib
	make $(EXEC)

$(EXEC): $(MAIN_O) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) -lqtc

$(MAIN_O): $(MAIN_C)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

doxygen: 
	doxygen Doxyfile

clean:
	rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
	rm -rf docs
	make -f Makelib clean

.PHONY: all clean
