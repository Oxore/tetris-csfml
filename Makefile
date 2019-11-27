Q=@
QQ=@

LD=$(CC)

TARGET_TETRIS:=tetris
TARGET_TEST:=test
CJSON:=deps/cJSON
MUNIT:=deps/munit
LIBF8:=deps/libf8

ifeq ($(wildcard $(MUNIT)/*),)
NOTEST=1
endif

SRC:=src
BUILD:=build
TARGET:=$(BUILD)/target
MEDIA:=media
SOURCES:=$(wildcard $(SRC)/*.c)
SOURCES+=$(wildcard $(SRC)/$(MEDIA)/*.c)
OBJECTS:=$(SOURCES:$(SRC)/%.c=$(BUILD)/%.c.o)
DEPENDS:=$(OBJECTS:.o=.d)

ifdef PREFIX
	INCLUDE+=$(PREFIX)/include
endif
INCLUDE+=include
INCLUDE+=$(CJSON)
INCLUDE+=$(MUNIT)
INCLUDE+=$(LIBF8)
INCLUDE:=$(INCLUDE:%=-I%)

#COMMON+=-fsanitize=address

CFLAGS+=$(COMMON)
CFLAGS+=$(INCLUDE)
CFLAGS+=-Wall
CFLAGS+=-Wextra
CFLAGS+=-Wpedantic
CFLAGS+=-Wno-microsoft-anon-tag
# CSFML headers used SFML 2.5.1 have some deprecated structs involved
CFLAGS+=-Wno-deprecated-declarations
CFLAGS+=-std=c11
CFLAGS+=-fms-extensions
CFLAGS+=-g3
CFLAGS+=-Og
CFLAGS+=-MD
CFLAGS+=$(EXTRA_CFLAGS)

ifdef PREFIX
	LDFLAGS+=-L$(PREFIX)/lib
endif
LDFLAGS+=$(COMMON)
LDFLAGS+=$(EXTRA_LDFLAGS)

ifdef SFML_STATIC
	LDFLAGS+=-lsfml-graphics-s
	LDFLAGS+=-lsfml-window-s
	LDFLAGS+=-lsfml-system-s
	ifeq ($(OS),Windows_NT)
		CFLAGS+=-DCSFML_STATIC
		LDFLAGS+=-lstdc++
		LDFLAGS+=-static
		LDFLAGS+=-static-libgcc
		LDFLAGS+=-static-libstdc++
		LDFLAGS+=-lfreetype
		LDFLAGS+=-lopengl32
		LDFLAGS+=-lgdi32
		LDFLAGS+=-lwinmm
		LDFLAGS+=-mwindows
	else
		UNAME_S:=$(shell uname -s)
		ifeq ($(UNAME_S),Linux)
			LDFLAGS+=-pthread
			LDFLAGS+=-lstdc++
			LDFLAGS+=-lm
			LDFLAGS+=-lfreetype
			LDFLAGS+=-lX11
			LDFLAGS+=-lXrandr
			LDFLAGS+=-ldl
			LDFLAGS+=-lGL
			LDFLAGS+=-ludev
		endif
		ifeq ($(UNAME_S),Darwin)
			LDFLAGS+=-framework OpenGL
			LDFLAGS+=-framework AppKit
			LDFLAGS+=-framework IOKit
			LDFLAGS+=-framework Carbon
			LDFLAGS+=-lfreetype
			LDFLAGS+=-lc++
			LDFLAGS+=-ObjC
		endif
	endif # ($(OS),Windows_NT)
else # SFML_STATIC
	ifdef PREFIX
		LDFLAGS+=-Wl,-rpath=$(PREFIX)/lib
	endif
endif # SFML_STATIC

LDFLAGS_TETRIS+=-lcsfml-graphics
LDFLAGS_TETRIS+=-lcsfml-window
LDFLAGS_TETRIS+=-lcsfml-system
LDFLAGS_TETRIS+=$(LDFLAGS)

LDFLAGS_TEST+=$(LDFLAGS)

#======================================================================

all:
all: $(TARGET_TETRIS)
ifndef NOTEST
all: $(TARGET_TEST)
endif

.PHONY: pg
pg:
	$(MAKE) -f pg.mk $@ PREFIX=$(PREFIX)

$(TARGET_TETRIS): $(OBJECTS) $(TARGET)/$(TARGET_TETRIS).c.o $(LIBF8)/libf8.a \
	$(CJSON)/cJSON.o
	$(QQ) echo "  LD      $@"
	$(Q) $(LD) -o $@ $^ $(LDFLAGS_TETRIS)

$(TARGET_TEST): $(TARGET)/$(TARGET_TEST).c.o $(MUNIT)/munit.c.o
	$(QQ) echo "  LD      $@"
	$(Q) $(LD) -o $@ $^ $(LDFLAGS_TEST)

$(DEPENDS): | $(BUILD)/ $(TARGET)/
$(OBJECTS): | $(BUILD)/ $(TARGET)/ $(BUILD)/$(MEDIA)/

$(BUILD)/$(MEDIA)/:
	$(Q) mkdir -p $@

$(BUILD)/:
	$(Q) mkdir -p $@

$(TARGET)/:
	$(Q) mkdir -p $@

$(LIBF8)/libf8.a: $(LIBF8)
	$(MAKE) -C $< NOTEST=1

$(CJSON)/cJSON.c.o: $(CJSON)/cJSON.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

$(MUNIT)/munit.c.o: $(MUNIT)/munit.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.c.o: $(SRC)/%.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

-include $(DEPENDS)

clean:
	$(Q) $(RM) -rfv $(OBJECTS) $(DEPENDS) \
		$(TARGET_TETRIS) $(TARGET_TEST) \
		$(TARGET)/$(TARGET_TETRIS).c.o $(TARGET)/$(TARGET_TETRIS).c.d \
		$(TARGET)/$(TARGET_TEST).c.o $(TARGET)/$(TARGET_TEST).c.d
	$(MAKE) -f pg.mk $@

mrproper: clean
	$(Q) $(RM) -rfv $(BUILD) \
		$(MUNIT)/*.d $(MUNIT)/*.o \
		$(CJSON)/*.d $(CJSON)/*.o
	$(MAKE) -C $(LIBF8) clean

.PHONY: all clean mrproper
