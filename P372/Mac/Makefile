CC = gcc
source_dir = ../Src/P372/
bin_dir = ../Bin/
# Use the following for testing
#CFLAGS = -fPIC -Wall -Wextra -O0 -g -I$(source_dir)
# Use the following for production
CFLAGS = -fPIC -Wall -Wextra -O2 -I$(source_dir)

LDFLAGS = -shared -lm -ldl 
RM = rm -f
TARGET_LIB = libp372.so

prefix ?= /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib

SRCS = $(source_dir)InitializeNoise.c \
       $(source_dir)Noise.c \
       $(source_dir)NoiseMemory.c

OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@
include $(SRCS:.c=.d)

.PHONY: clean

clean:
	${RM} ${OBJS} $(SRCS:.c=.d)
	${RM} ${TARGET_LIB}

install: all
	install -d $(DESTDIR)$(libdir)/
	install -m 755 $(TARGET_LIB) $(DESTDIR)$(libdir)/$(TARGET_LIB)
