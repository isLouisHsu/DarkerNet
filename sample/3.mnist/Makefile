# directories & target name
OBJDIR = .
DARKNET = /usr/local/darknet
TARGET  = mnist

# compile macro  
CC      = gcc
CFLAGS  = -g -Wall
LDFLAGS = -lm -pthread 

# load source files
SRC = $(wildcard *.c)

COMMON += -I ${DARKNET}/include/ -I ${DARKNET}/src/ -I .
LDFLAGS += -L ${DARKNET}/lib/ -ldarknet

# build target
OBJ = $(patsubst %.c, ${OBJDIR}/%.o, ${notdir ${SRC}})
EXE = ${TARGET}

# build
${EXE}: ${OBJ}
	$(CC) $(COMMON) $(OBJ) -o $@ $(LDFLAGS)
${OBJDIR}/%.o: %.c
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@ 

# clean
.PHONY: clean
clean:
	find ${OBJDIR} -name *.o -exec rm -rf {} \;
