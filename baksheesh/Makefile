CC=g++
OPT= -O3 -Wno-format
LIB=-lm -lpthread

HED=$(shell ls *.h)

all: byte_fault
byte_fault : attack.cpp $(HED)
	$(CC) -DNOCYTHON=1 $(OPT) -o byte_fault  attack.cpp $(LIB)
	
clean:
	rm -f byte_fault

