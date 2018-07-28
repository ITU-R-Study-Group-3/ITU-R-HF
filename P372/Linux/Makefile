CC = gcc
source_dir = ../Src/P372/
bin_dir = ../Bin/
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I$(source_dir)
LDFLAGS = -shared -lm -ldl 
RM = rm -f
TARGET_LIB = $(bin_dir)libp372.so

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

install: all
	install -m 644 $(TARGET_LIB) $(DESTDIR)$(PREFIX)/lib/
