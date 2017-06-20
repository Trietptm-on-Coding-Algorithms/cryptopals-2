CC = gcc
CFLAGS = -g -Wall

.PHONY: all clean

SET = $(wildcard set*)
SRC = $(wildcard $(SET)/*.c)
OBJ = $(patsubst %.c, %, $(SRC))

all: $(OBJ)

clean: 
	-rm $(OBJ)
