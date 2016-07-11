.PHONY: all clean

CC = g++

CFLAGS = -c
LDFLAGS+ = -lm     # link to math library
OPTFLAGS = -O3

all: test train
	@echo "Compilation Success!"

test: test.cpp hmm.h
	$(CC) $(OPTFLAGS) $< -o test

train: train.cpp hmm.h
	$(CC) $(OPTFLAGS) $< -o train

clean:
	rm -rf test train   # type make clean to remove the compiled file