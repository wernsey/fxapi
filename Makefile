CC=gcc
CFLAGS=-c -Wall -DUSESTB -I./include -I./extra
LDFLAGS=-lm

# TODO: Remove this one things stabilize
BUILD=debug

SRC_DIR := src
OBJ_DIR := obj

OBJECTS=$(SOURCES:.c=.o)
TEST1=test1/app
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
  TEST1:=$(TEST1).exe
endif

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TEST1) $(LIB)

lib: $(LIB)

debug:
	$(MAKE) BUILD=debug

$(TEST1): test1/main.o $(LIB)
	@echo $@
	@$(CC) $^ $(LDFLAGS) -o $@

$(LIB): $(OBJ)
	@echo $@
	@ar rsc $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo $@
	@$(CC) $(CFLAGS) -c $< -o $@

test1/%.o: test1/%.c
	@echo $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@echo $@
	@mkdir -p $@

# Add header dependencies here

$(OBJ_DIR)/bm_cache.o: src/bm_cache.c extra/bmph.h include/fx.h include/bm_cache.h
$(OBJ_DIR)/fx.o: src/fx.c extra/glmatrix.h extra/bmph.h include/fx.h
$(OBJ_DIR)/md2.o: src/md2.c include/md2.h include/fx.h
$(OBJ_DIR)/md5.o: src/md5.c extra/glmatrix.h include/md5.h extra/bmph.h include/fx.h \
 include/bm_cache.h
$(OBJ_DIR)/mdl.o: src/mdl.c extra/bmph.h include/mdl.h include/fx.h
$(OBJ_DIR)/obj.o: src/obj.c include/fx.h include/obj.h
$(OBJ_DIR)/shapes.o: src/shapes.c include/fx.h include/shapes.h extra/par_shapes.h
test1/main.o: test1/main.c extra/bmph.h include/fx.h extra/glmatrix.h

.PHONY : clean dist

dist: dist.zip
dist.zip: clean
	@zip -r $@ *

deps:
	@$(CC) -MM -I ./include -I ./extra $(SRC) |  sed 's/\(.*\.o:\)/$$(OBJ_DIR)\/\1/'
	@$(CC) -MM -I ./include -I ./extra test1/*.c |  sed 's/\(.*\.o:\)/test1\/\1/'

clean:
	@echo Cleaning...
	@-rm -f $(LIB)
	@-rm -rf $(OBJ_DIR) $(WEB_DIR)
	@-rm -f $(TEST1) test1/*.o
	@-rm -f out.gif pick.gif
	@-rm -f dist.zip
