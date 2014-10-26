CC=gcc
RM=rm
# build an executable named utdash from utdash.c
all: utdash.c
	$(CC) -g -Wall -o utdash utdash.c
clean:
	$(RM) utdash
