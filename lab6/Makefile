CC = gcc
CCFLAGS = -Wall -Wextra -ansi -pedantic -O2
all: servidor cliente

servidor:
	$(CC) $(CFLAGS) -o servidor servidor_telnet.c netutils.c

cliente:
	$(CC) $(CFLAGS) -o cliente cliente_telnet.c netutils.c
