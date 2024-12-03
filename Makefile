symac: src/symac
	cp src/symac .

src/symac:
	$(MAKE) -C src symac

ifeq ($(PREFIX),)
  PREFIX:=/usr
endif

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install src/symac $(DESTDIR)$(PREFIX)/bin/symac

.PHONY: clean
clean:
	$(MAKE) -C src clean

