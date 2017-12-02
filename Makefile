CC=gcc
CFLAGS=-g -Wall

all:
	${CC} keygen.c -o keygen ${CFLAGS}

keygen:
	${CC} keygen.c -o keygen ${CFLAGS}

clean:
	rm keygen
