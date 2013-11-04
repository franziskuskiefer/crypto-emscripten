
## Created by Anjuta

CC = clang -v #gcc
CFLAGS = -g -Wall
OBJECTS = spake.o util.o test.o
INCFLAGS = 
LDFLAGS = #-Wl,-rpath,../build/lib/ -L../build/lib/
LIBS = -lgcrypt -lgpg-error

all: spake

spake: $(OBJECTS)
	$(CC) -o spake $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o
	rm spake

.PHONY: all
.PHONY: count
.PHONY: clean
