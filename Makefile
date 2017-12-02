CC=gcc
CFLAGS+=-Wall
CFLAGS+=-std=c11
CFLAGS+=-O0
CFLAGS+=-I$(INCLUDE)
LIBS+=-lcsfml-graphics
LIBS+=-lcsfml-window
LIBS+=-lcsfml-system
LIBS+=-lyaml

BUILD:=build
SRC:=src
INCLUDE:=include
SOURCES:=$(wildcard $(SRC)/*.c)
OBJECTS:=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))

TARGET:=tetris

#======================================================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Compiling: $@"
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@echo "Build successfull"

$(OBJECTS): | $(BUILD)

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c
	@echo "Compiling: $@"
	@$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

clean:
	@rm -rfv $(TARGET) $(BUILD)

.PHONY: all clean
