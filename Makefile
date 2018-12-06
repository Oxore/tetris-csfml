Q=@
QQ=@

TARGET_TETRIS:=tetris
TARGET_TEST:=test
MUNIT:=deps/munit

ifeq ($(wildcard $(MUNIT)/*),)
NOTEST=1
endif

SRC:=src
BUILD:=build
TARGET:=$(BUILD)/target
SOURCES:=$(wildcard $(SRC)/*.c)
OBJECTS:=$(SOURCES:$(SRC)/%.c=$(BUILD)/%.c.o)
DEPENDS:=$(OBJECTS:.o=.d)

INCLUDE+=$(PREFIX)/include
INCLUDE+=include
INCLUDE+=$(MUNIT)
INCLUDE:=$(INCLUDE:%=-I%)

#COMMON+=-fsanitize=address

CFLAGS+=$(COMMON)
CFLAGS+=$(INCLUDE)
CFLAGS+=-Wall
CFLAGS+=-Wextra
CFLAGS+=-Wpedantic
CFLAGS+=-Wno-microsoft-anon-tag
CFLAGS+=-std=c11
CFLAGS+=-fms-extensions
CFLAGS+=-g3
CFLAGS+=-O0
CFLAGS+=-MD

LDFLAGS+=-L$(PREFIX)/lib
LDFLAGS+=-Wl,-rpath=$(PREFIX)/lib
LDFLAGS+=$(COMMON)

LDFLAGS_TETRIS+=$(LDFLAGS)
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

$(TARGET_TETRIS): $(OBJECTS) $(TARGET)/$(TARGET_TETRIS).c.o
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS_TETRIS)

$(TARGET_TEST): $(TARGET)/$(TARGET_TEST).c.o $(MUNIT)/munit.c.o $(BUILD)/unicode.c.o
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS_TEST)

$(DEPENDS): | $(BUILD)
$(OBJECTS): | $(BUILD)

$(BUILD):
	$(Q) mkdir -p $@

$(TARGET):
	$(Q) mkdir -p $@

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
