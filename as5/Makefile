OUTFILE = ftd
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
LFLAGS = -L$(HOME)/cmpt433/public/as5

all:
	$(CC_C) $(CFLAGS) main.c ftd.c -o $(OUTDIR)/$(OUTFILE) $(LFLAGS)

app:
	$(CC_C) $(CFLAGS) main.c ftd.c $(OUTDIR)/$(OUTFILE)

clean:
	rm $(OUTDIR)/$(OUTFILE)