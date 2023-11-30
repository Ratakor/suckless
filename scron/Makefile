CFLAGS += -std=c99 -Wall -Wextra -pedantic -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

BIN = scron
MAN = scron.1

all: $(BIN)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(BIN)
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f $(MAN) $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(MAN)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)\
		$(DESTDIR)$(MANPREFIX)/man1/$(MAN)

clean:
	rm -f $(BIN)

.PHONY: all install uninstall clean
