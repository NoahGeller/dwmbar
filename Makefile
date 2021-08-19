CC?=cc
CFLAGS=-O2
LDFLAGS=-lX11

.PHONY: all, clean, install, uninstall

all: dwmbar

dwmbar: dwmbar.c modules.h
	$(CC) $(CFLAGS) dwmbar.c $(LDFLAGS) -o dwmbar


clean:
	rm -f dwmbar

install: dwmbar
	cp $< /usr/local/bin

uninstall:
	rm -f /usr/local/bin/dwmbar
