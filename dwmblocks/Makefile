PREFIX  ?= /usr/local
BLOCKS   = \\\"${PREFIX}/share/dwmblocks/blocks\\\"
CFLAGS  += -D_DEFAULT_SOURCE -O3
LDFLAGS += -s -lX11 -lconfig -pthread

all: dwmblocks

dwmblocks: dwmblocks.c
	${CC} $< ${CFLAGS} ${LDFLAGS} -o $@

clean:
	rm -f dwmblocks

install: clean
	@CFLAGS="$$CFLAGS -DBLOCKS=${BLOCKS}" ${MAKE}
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwmblocks ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwmblocks
	mkdir -p ${DESTDIR}${PREFIX}/share/dwmblocks
	cp -rf blocks ${DESTDIR}${PREFIX}/share/dwmblocks
	chmod 755 ${DESTDIR}${PREFIX}/share/dwmblocks/blocks/*

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/dwmblocks\
		${DESTDIR}${PREFIX}/share/dwmblocks

.PHONY: all clean install uninstall
