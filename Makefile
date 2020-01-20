CC    = gcc
SRC   = $(wildcard *.c)
OBJ   = $(SRC:.c=.o)
HDR   = $(wildcard *.h)
CFLAGS = -g -std=c11

.PHONY: clean

a.out: $(OBJ) $(HDR)
	gcc -g $(OBJ) -lm

clean:
	rm -f $(OBJ) foo
