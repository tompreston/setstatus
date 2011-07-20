CC=gcc
CFLAGS=-Wall -g -std=c99 -pedantic-errors -D_GNU_SOURCE
LDFLAGS=-l X11 -l mpdclient -l m -l pthread

setstatus: setstatus.o battery.o mpdinfo.o
	$(CC) $(CFLAGS) $(LDFLAGS) setstatus.o battery.o mpdinfo.o -o setstatus
	rm *.o

mpdinfo.o: mpdinfo.c
	$(CC) $(CFLAGS) -c mpdinfo.c

battery.o: battery.c
	$(CC) $(CFLAGS) -c battery.c

setstatus.o: setstatus.c
	$(CC) $(CFLAGS) -c setstatus.c
