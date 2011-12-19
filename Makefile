CC=g++
CFLAGS=-O3 -funroll-loops -c -std=c++0x
LDFLAGS=-O2 -lm
SOURCES=main.cpp
OBJECTS=$(addsuffix .o, $(basename ${SOURCES}))
EXECUTABLE=test

#Uncomment the following to enable debugging
#CFLAGS+=-g -DDEBUG

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o: *.h
	$(CC) $(CFLAGS) $< -o $@

.cpp.o: *.h
	$(CC) $(CFLAGS) $< -o $@

clean: 
	-rm -f ${EXECUTABLE} ${OBJECTS} *.d
	-rm -f debug.txt

.PHONY: all clean

