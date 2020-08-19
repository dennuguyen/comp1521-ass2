CC     = clang
CFLAGS = -Wall -I.

smips: smips.o
	$(CC) $(CFLAGS) smips.c functions.c hardware.c opcode.c -o smips

clean:
	-rm -f smips.o
	-rm -f smips