CC         = wcl
CPU_FLAGS  ?= -3
OPT_FLAGS  ?= -ox -ot
# The program fits in one 64 KiB code segment and one 64 KiB data segment.
# Small model keeps normal code/data pointers near; huge model makes every
# array subscript perform expensive normalized far-pointer arithmetic.
CFLAGS     = -bt=dos -ms -zq $(CPU_FLAGS) $(OPT_FLAGS)
TARGET     = ZINE.EXE

SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,build/%.obj,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -fe=$@

build/%.obj: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -fo=$@

# Keep the shipped ZINE.EXE when cleaning. A successful link replaces it.
clean:
	rm -f build/*.obj

.PHONY: all clean
