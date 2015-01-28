CC=gcc
DEBUG=-g3
CURRENT=$(shell uname -r)
CFLAGS=-c $(DEBUG) -Wall -D_REENTRANT
$(LDFLAGS_NOKVERSION)=
INCLUDE=-I../
SOURCES=webadc.c
OBJECTS=$(SOURCES:.c=.o) libshttpd.a
EXECUTABLE=webadc
LDLOADLIBS=-ldl

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $($(LDFLAGS_NOKVERSION)) $(OBJECTS) -o $@ $(LDLOADLIBS)

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) $< -o $@
clean:
	rm -rf *.o webadc *.data *.gnuplot *.png
