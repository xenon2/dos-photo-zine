# ==== Makefile ====
CC      = wcl
CFLAGS  = -bt=dos -mh -zq -ol
TARGET  = main.exe

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