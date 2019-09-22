# playground target for trying things with SFML

LIBF8=deps/libf8

ifneq ($(PREFIX),)
LDFLAGS+=-L$(PREFIX)/lib
LDFLAGS+=-Wl,-rpath=$(PREFIX)/lib
INCLUDE+=-I$(PREFIX)/include
endif

INCLUDE+=-I$(LIBF8)

LDFLAGS+=-lcsfml-graphics
LDFLAGS+=-lcsfml-window
LDFLAGS+=-lcsfml-system
LDFLAGS+=-lyaml

CFLAGS+=-Wno-deprecated-declarations

pg: src/target/pg.c $(LIBF8)/libf8.a
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE) $^ -o $@

$(LIBF8)/libf8.a: $(LIBF8)
	make -C $< NOTEST=1

clean:
	rm -rfv pg
