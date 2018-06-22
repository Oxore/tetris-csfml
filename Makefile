Q=@
QQ=@

TARGET:=tetris

BUILD:=build
SRC:=src
SOURCES:=$(wildcard $(SRC)/*.c)
OBJECTS:=$(patsubst $(SRC)/%.c,$(BUILD)/%.c.o,$(SOURCES))

INCLUDE+=include
INCLUDE:=$(patsubst %,-I%,$(INCLUDE))

CFLAGS+=$(INCLUDE)
CFLAGS+=-Wall
CFLAGS+=-std=c11
CFLAGS+=-g3
CFLAGS+=-O0

LDFLAGS+=-lcsfml-graphics
LDFLAGS+=-lcsfml-window
LDFLAGS+=-lcsfml-system
LDFLAGS+=-lyaml

#======================================================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS)

$(OBJECTS): | $(BUILD)

$(BUILD):
	$(Q) mkdir -p $(BUILD)

$(BUILD)/%.c.o: $(SRC)/%.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

clean:
	$(Q) $(RM) -rfv $(TARGET) $(BUILD)

.PHONY: all clean
