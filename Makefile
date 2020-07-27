CC     = clang
CFLAGS = -Werror -fsanitize=address

smips: smips.o
	$(CC) $(CFLAGS) smips.c -o smips

clean:
	-rm -f smips.o
	-rm -f smips