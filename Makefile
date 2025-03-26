CC=		gcc
CFLAGS+=	-Ilocal/include -O0 -g -lpthread
CFLAGS+=	-Wall -Werror -DSET_CPU_AFFINITY
HEADERS!=	ls src/*.h
REMOTE=		"rpi:~/demo/"

all: ccat bench.sc bench.opt

clean:
	rm -rf ccat bench.* *.ll src/*.ll *.jpg *.core output

ccat: src/ccat.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

bench.sc: src/bench.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ src/bench.c

bench.opt: src/bench.c $(HEADERS)
	$(CC) $(CFLAGS) -DOPTIMIZED -o $@ src/bench.c

upload:
	rsync -zaP . $(REMOTE)

.PHONY: all clean upload
