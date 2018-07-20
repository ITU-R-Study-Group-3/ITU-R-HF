CC = gcc
source_dir = ../src/P372/
bin_dir = ../../bin/
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I$(source_dir)
LDFLAGS = -shared -lm -ldl 
RM = rm -f
TARGET_LIB = $(bin_dir)libp372.so

SRCS = $(source_dir)InitializeNoise.c \
       $(source_dir)Noise.c \
       $(source_dir)NoiseMemory.c

DEPS = $(source_dir)common.h \
       $(source_dir)Noise.h

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
