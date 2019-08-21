CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm 

# TODO: Remove this one things stabilize
BUILD=debug

# Add your source files here:
SOURCES= bmp.c fx.c obj.c md2.c \
	gl-matrix/mat3.c gl-matrix/quat.c gl-matrix/mat4.c \
	gl-matrix/vec2.c gl-matrix/vec3.c gl-matrix/vec4.c \
	gl-matrix/str.c

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
main.o : main.c fx.h gl-matrix/gl-matrix.h fx.h
bmp.o : bmp.c bmp.h
fx.o: fx.c fx.h bmp.h gl-matrix/gl-matrix.h fx.h
obj.o : obj.c obj.h gl-matrix/gl-matrix.h fx.h
md2.o : md2.h gl-matrix/gl-matrix.h fx.h

.PHONY : clean dist

dist: dist.zip
dist.zip: clean
	zip -r $@ *

clean: sweep
	-rm -f $(LIB)
	-rm -f $(EXECUTABLE) $(EXECUTABLE).exe
	
sweep:
	-rm -f *.o ./gdi/*.o gl-matrix/*.o
	-rm -f *.log
	-rm -f dist.zip screen-*.bmp
