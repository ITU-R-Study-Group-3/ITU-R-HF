UNAME_S := $(shell uname -s)

# Set CC based on the operating system
ifeq ($(UNAME_S),Darwin)
    CC = clang
else
    CC = gcc
endif

source_dir = ../Src/P533/
bin_dir = ../Bin/
# Use the following for testing
#CFLAGS = -fPIC -Wall -Wextra -O0 -g -I$(source_dir)
# Use the following for production
CFLAGS = -std=c99 -fPIC -Wall -Wextra -O2 -I$(source_dir)

# Set linker flags based on the operating system
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -shared -lm -ldl
else
    LDFLAGS = -shared -lm -ldl -z muldefs
endif

RM = rm -f
TARGET_LIB = libp533.so

prefix ?= /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib

SRCS = $(source_dir)Between7000kmand9000km.c \
       $(source_dir)ELayerScreeningFrequency.c \
       $(source_dir)Magfit.c \
	   $(source_dir)MedianSkywaveFieldStrengthShort.c \
	   $(source_dir)ReadIonParameters.c \
	   $(source_dir)CalculateCPParameters.c \
	   $(source_dir)Geometry.c \
	   $(source_dir)MUFBasic.c \
	   $(source_dir)P533.c \
	   $(source_dir)MUFOperational.c \
	   $(source_dir)ReadP1239.c \
	   $(source_dir)CircuitReliability.c \
	   $(source_dir)InitializePath.c \
	   $(source_dir)MedianAvailableReceiverPower.c \
	   $(source_dir)ReadType13.c \
	   $(source_dir)InputDump.c \
	   $(source_dir)MedianSkywaveFieldStrengthLong.c \
	   $(source_dir)MUFVariability.c \
	   $(source_dir)PathMemory.c \
	   $(source_dir)ValidatePath.c

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
