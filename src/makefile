OBJS = fsm.o
CC = g++
DEBUG = -g
WARNINGS = -Wno-unused-variable
LFLAGS = -Wno-unused-variable
CFLAGS = $(LFLAGS) -c $(DEBUG)

all: output

output: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o fsm

fsm.o: fsm.cc
	$(CC) $(CFLAGS) fsm.cc

clean:
	rm *.o fsm

mealy:
	./fsm