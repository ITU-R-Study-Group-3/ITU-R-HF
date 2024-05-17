UNAME_S := $(shell uname -s)

# Set CC based on the operating system
ifeq ($(UNAME_S),Darwin)
    CC = clang
else
    CC = gcc
endif

CFLAGS = -fPIC -Wall -Wextra -O2 -g
LDFLAGS = -lm -ldl
RM = rm -f
TARGET_LIB = ITURHFPROP 

SRCS = DumpPathData.c  ITURHFProp.c  ReadInputConfiguration.c  Report.c  ValidateITURHFP.c
DEPS = ITURHFProp.h Common.h Noise.h P533.h
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
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)

