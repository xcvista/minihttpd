#
# GNUmakefile
# minihttpd
#
# Written by Maxthon Chan
#
# This is a generic Makefile that automatically detects the system. Generally
# you should not need to edit it.
#

TARGET := minihttpd

CFLAGS += -std=gnu11 -fPIC -Wall
CPPFLAGS += -Iinclude
LDFLAGS += -lpthread -lm

C_FILES := $(wildcard src/*.c)
HEADERS := $(wildcard include/*.h) include/version.h

CC := $(shell ./compiler.sh)
CCLD := $(CC)

OBJS := $(patsubst %.c,%.c.o,$(C_FILES))

all: $(TARGET)
.PHONY:all clean check

$(TARGET): $(OBJS)
	$(CCLD) $(LDFLAGS) $(OBJS) -o $@

%.c.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

include/version.h: version.sh
	./version.sh > $@

clean:
	-rm -rf $(OBJS) $(TARGET) include/version.h

