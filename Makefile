CC     = clang
CFLAGS = -Werror -fsanitize=address

smips: smips.o
	$(CC) $(CFLAGS) smips.o -o smips

smips.o: smips.c
	$(CC) -c $(CFLAGS) smips.c -o smips.o

clean:
	-rm -f smips.o
	-rm -f smips