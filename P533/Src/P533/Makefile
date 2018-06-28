CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I../../../P372/Src/P372/
LDFLAGS = -shared -lm -ldl #-L../../../P372/Src/P372/ -llibp372.so
RM = rm -f
TARGET_LIB = libp533.so

SRCS = Between7000kmand9000km.c ELayerScreeningFrequency.c Magfit.c MedianSkywaveFieldStrengthShort.c ReadIonParameters.c \
	CalculateCPParameters.c Geometry.c MUFBasic.c P533.c MUFOperational.c ReadP1239.c \
	CircuitReliability.c InitializePath.c MedianAvailableReceiverPower.c ReadType13.c \
	InputDump.c MedianSkywaveFieldStrengthLong.c MUFVariability.c PathMemory.c ValidatePath.c
DEPS = Common.h Noise.h P533.h
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
