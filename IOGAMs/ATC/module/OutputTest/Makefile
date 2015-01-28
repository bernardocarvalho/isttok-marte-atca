CC=gcc
DEBUG=-g3
CURRENT=$(shell uname -r)
CFLAGS=-c $(DEBUG) -Wall -D_REENTRANT 
$(LDFLAGS_NOKVERSION)=
SOURCES=OutputTest.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=OutputTest
LDLOADLIBS=

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $($(LDFLAGS_NOKVERSION)) $(OBJECTS) -o $@ $(LDLOADLIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *.o OutputTest
