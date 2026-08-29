# ==== Makefile ====
CC         = wcl
CPU_FLAGS  ?= -3
OPT_FLAGS  ?= -ox -ot
CFLAGS     = -bt=dos -mh -zq $(CPU_FLAGS) $(OPT_FLAGS)
TARGET     = main.exe

SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,build/%.obj,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -fe=$@


build/%.obj: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -fo=$@

clean:
	rm -f build/*.obj $(TARGET)

.PHONY: all clean