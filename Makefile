Q=@
QQ=@

TARGET_TETRIS:=tetris
TARGET_TEST:=test
MUNIT:=deps/munit
LIBF8:=deps/libf8

ifeq ($(wildcard $(MUNIT)/*),)
NOTEST=1
endif

SRC:=src
BUILD:=build
TARGET:=$(BUILD)/target
SOURCES:=$(wildcard $(SRC)/*.c)
OBJECTS:=$(SOURCES:$(SRC)/%.c=$(BUILD)/%.c.o)
DEPENDS:=$(OBJECTS:.o=.d)

ifdef PREFIX
INCLUDE+=$(PREFIX)/include
endif
INCLUDE+=include
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
CFLAGS+=-O0
CFLAGS+=-MD

ifdef PREFIX
LDFLAGS+=-L$(PREFIX)/lib
LDFLAGS+=-Wl,-rpath=$(PREFIX)/lib
endif
LDFLAGS+=$(COMMON)

LDFLAGS_TETRIS+=$(LDFLAGS)
LDFLAGS_TETRIS+=-L$(LIBF8)
LDFLAGS_TETRIS+=-l:libf8.a
LDFLAGS_TETRIS+=-lcsfml-graphics
LDFLAGS_TETRIS+=-lcsfml-window
LDFLAGS_TETRIS+=-lcsfml-system
LDFLAGS_TETRIS+=-lyaml

LDFLAGS_TEST+=$(LDFLAGS)

#======================================================================

all:
all: $(TARGET)
all: $(TARGET_TETRIS)
ifndef NOTEST
all: $(TARGET_TEST)
endif

$(TARGET_TETRIS): $(OBJECTS) $(TARGET)/$(TARGET_TETRIS).c.o $(LIBF8)/libf8.a
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS_TETRIS)

$(TARGET_TEST): $(TARGET)/$(TARGET_TEST).c.o $(MUNIT)/munit.c.o
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS_TEST)

$(DEPENDS): | $(BUILD)
$(OBJECTS): | $(BUILD)

$(BUILD):
	$(Q) mkdir -p $@

$(TARGET):
	$(Q) mkdir -p $@

$(LIBF8)/libf8.a: $(LIBF8)
	make -C $<

$(MUNIT)/munit.c.o: $(MUNIT)/munit.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.c.o: $(SRC)/%.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

-include $(DEPENDS)

clean:
	$(Q) $(RM) -rfv $(TARGET_TETRIS) $(TARGET_TEST) $(BUILD)
	$(Q) $(RM) -rfv $(MUNIT)/*.d $(MUNIT)/*.o

.PHONY: all clean
