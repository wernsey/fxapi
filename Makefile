CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm

# TODO: Remove this one things stabilize
BUILD=debug

# Add your source files here:
SOURCES=main.c bmp.c fx.c \
	gl-matrix/mat3.c gl-matrix/quat.c gl-matrix/mat4.c \
	gl-matrix/vec2.c gl-matrix/vec3.c gl-matrix/vec4.c \
	gl-matrix/str.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=app.exe

ifeq ($(BUILD),debug)
# Debug
CFLAGS += -O0 -g
LDFLAGS +=
else
# Release mode
CFLAGS += -O2 -DNDEBUG
LDFLAGS += -s
endif

all: $(EXECUTABLE)

debug:
	make BUILD=debug

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
# Add header dependencies here
main.o : main.c fx.h
bmp.o : bmp.c bmp.h
fx.o: fx.c fx.h bmp.h

.PHONY : clean dist

dist: dist.zip
dist.zip: clean
	zip -r $@ *

clean: sweep
	-rm -f $(EXECUTABLE)
	
sweep:
	-rm -f *.o ./gdi/*.o gl-matrix/*.o
	-rm -f *.log
	-rm -f dist.zip screen-*.bmp
