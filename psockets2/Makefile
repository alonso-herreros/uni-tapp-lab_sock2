all: TCPechod TCPecho
noreaper: TCPechod_noreaper

CFLAGS=-include /usr/include/errno.h 

TCPechod: TCPechod.c errexit.c 

TCPecho: TCPecho.c errexit.c 

TCPechod_noreaper: TCPechod.c errexit.c
	$(CC) $(CFLAGS) -DNOREAPER TCPechod.c errexit.c -o TCPechod_noreaper

clean:
	rm -f *.o TCPecho TCPechod TCPechod_noreaper
