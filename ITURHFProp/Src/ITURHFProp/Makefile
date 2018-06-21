CC = gcc # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I../p533# C flags
LDFLAGS = -lm -ldl # linking flags
RM = rm -f  # rm command
TARGET = ITURHFProp# target lib
 
SRCS = \
DumpPathData.c  ITURHFProp.c  \
ReadInputConfiguration.c  \
ValidateITURHFP.c  \
Report.c

OBJS = $(SRCS:.c=.o)
 
.PHONY: all
all: ${TARGET}
 
$(TARGET): $(OBJS)
	$(CC)  -o $@ $^ ${LDFLAGS}	

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@
 
#include $(SRCS:.c=.d)
 
.PHONY: clean
clean:
	-${RM} ${TARGET} ${OBJS} $(SRCS:.c=.d)
