CC     = clang
CFLAGS = -Wall -I.

all: clean smips

smips: smips.o
	$(CC) $(CFLAGS) smips.c functions.c hardware.c hashtable.c opcode.c -o smips

clean:
	-rm -f smips.o
	-rm -f smips