CC=gcc
CFLAGS=-Wall -g -std=c99
LDFLAGS=-l X11

dwmstatus: dwmstatus.o battery.o
	$(CC) $(CFLAGS) $(LDFLAGS) dwmstatus.o battery.o -o dwmstatus
	rm *.o

dwmstatus.o: dwmstatus.c
	$(CC) $(CFLAGS) -c dwmstatus.c

test: test.o battery.o
	$(CC) $(CFLAGS) test.o battery.o -o test
	rm *.o

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

battery.o: battery.c
	$(CC) $(CFLAGS) -c battery.c
