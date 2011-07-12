CC=gcc
CFLAGS=-Wall -g -std=c99 -pedantic-errors -D_GNU_SOURCE
LDFLAGS=-l X11 -l mpdclient -l m

dwmstatus: dwmstatus.o battery.o mpdinfo.o
	$(CC) $(CFLAGS) $(LDFLAGS) dwmstatus.o battery.o mpdinfo.o -o dwmstatus
	rm *.o

mpdinfo.o: mpdinfo.c
	$(CC) $(CFLAGS) -c mpdinfo.c

battery.o: battery.c
	$(CC) $(CFLAGS) -c battery.c

dwmstatus.o: dwmstatus.c
	$(CC) $(CFLAGS) -c dwmstatus.c
