CC=gcc
CFLAGS=-Iinclude
SRCDIR=.
OUTDIR=bin/

all: epoll-server.c server.c events.c
	mkdir -p ${OUTDIR}
	${CC} ${CFLAGS} -o ${OUTDIR}/epoll-server ${SRCDIR}/epoll-server.c ${SRCDIR}/server.c ${SRCDIR}/events.c

clean:
	rm -rf ${OUTDIR}

