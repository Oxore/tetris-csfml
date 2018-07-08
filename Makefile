Q=@
QQ=@

TARGET:=tetris

BUILD:=build
SRC:=src
SOURCES:=$(wildcard $(SRC)/*.c)
OBJECTS:=$(SOURCES:$(SRC)/%.c=$(BUILD)/%.c.o)
DEPENDS:=$(OBJECTS:.o=.d)

INCLUDE+=include
INCLUDE:=$(INCLUDE:%=-I%)

COMMON+=-fsanitize=address

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
CFLAGS+=-MMD -MP

LDFLAGS+=$(COMMON)
LDFLAGS+=-lcsfml-graphics
LDFLAGS+=-lcsfml-window
LDFLAGS+=-lcsfml-system
LDFLAGS+=-lyaml

#======================================================================

all:
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(QQ) echo "  LD      $@"
	$(Q) $(CC) -o $@ $^ $(LDFLAGS)

$(DEPENDS): | $(BUILD)
$(OBJECTS): | $(BUILD)

$(BUILD):
	$(Q) mkdir -p $(BUILD)

$(BUILD)/%.c.o: $(SRC)/%.c
	$(QQ) echo "  CC      $@"
	$(Q) $(CC) -c $(CFLAGS) -o $@ $<

-include $(DEPENDS)

clean:
	$(Q) $(RM) -rfv $(TARGET) $(BUILD)

.PHONY: all clean
