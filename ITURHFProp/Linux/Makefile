CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2 -g
LDFLAGS = -lm -ldl
RM = rm -f
source_dir = ../Src/ITURHFProp/

prefix ?= /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

TARGET = ITURHFProp

SRCS = $(source_dir)DumpPathData.c\
	$(source_dir)ITURHFProp.c\
	$(source_dir)ReadInputConfiguration.c\
	$(source_dir)Report.c\
	$(source_dir)ValidateITURHFP.c

OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@
include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET} ${OBJS} $(SRCS:.c=.d)

install: all
	install -d $(DESTDIR)$(bindir)/
	install -m 755 $(TARGET) $(DESTDIR)$(bindir)/ITURHFProp
