CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm

# TODO: Remove this one things stabilize
BUILD=debug

# Add your source files here:
SOURCES= bmp.c fx.c obj.c md2.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=app
LIB=libfx.a

ifeq ($(BUILD),debug)
# Debug
CFLAGS += -O0 -g
LDFLAGS +=
else
# Release mode
CFLAGS += -O2 -DNDEBUG
LDFLAGS += -s
endif

ifeq ($(OS),Windows_NT)
  EXECUTABLE:=$(EXECUTABLE).exe
endif

all: $(EXECUTABLE)

lib: $(LIB)

debug:
	make BUILD=debug

$(EXECUTABLE): main.o $(LIB)
	$(CC) $^ $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(LIB): $(OBJECTS)
	ar rs $@ $^

# Add header dependencies here
main.o : main.c bmp.h fx.h glmatrix.h fx.h
bmp.o : bmp.c bmp.h
fx.o: fx.c fx.h bmp.h glmatrix.h fx.h
obj.o : obj.c obj.h fx.h
md2.o : md2.h md2.h fx.h

.PHONY : clean dist

dist: dist.zip
dist.zip: clean
	zip -r $@ *

clean: sweep
	-rm -f $(LIB)
	-rm -f $(EXECUTABLE)

sweep:
	-rm -f *.o ./gdi/*.o gl-matrix/*.o
	-rm -f out.gif pick.gif
	-rm -f dist.zip
