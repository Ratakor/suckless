PREFIX  ?= /usr/local

OBJ      = sb.o blocks.o util.o
CFLAGS  += -O3 -std=c99 -pedantic -Wall -Werror -D_DEFAULT_SOURCE
LDFLAGS += -s -lX11 -lpthread

all: sb

sb: config.h ${OBJ}
	${CC} ${OBJ} ${LDFLAGS} -o $@

config.h:
	cp config.def.h $@

clean:
	rm -f sb ${OBJ}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f sb sbcmd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/sb ${DESTDIR}${PREFIX}/bin/sbcmd

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/sb ${DESTDIR}${PREFIX}/bin/sbcmd

.PHONY: all clean install uninstall
