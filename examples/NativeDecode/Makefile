# Build the native (host) VictronBLE example.
#
#   make          build ./nativedecode
#   make run      build and decode the built-in sample advert
#   make clean
#
# No board, no PlatformIO, no toolchain beyond a C compiler — the library core
# is plain C99. Mirrors the build line in tests/vectors/run.sh.

ROOT    := ../..
CC      ?= cc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2
LDLIBS  := -lm

SRCS := \
	$(ROOT)/src/victronble_core.c \
	$(ROOT)/src/victronble_aes_sw.c \
	$(ROOT)/src/crypto/vble_aes.c \
	main.c

nativedecode: $(SRCS)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/src $(SRCS) $(LDLIBS) -o $@

run: nativedecode
	./nativedecode

clean:
	rm -f nativedecode

.PHONY: run clean
