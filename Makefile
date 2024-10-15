CC=gcc
CFLAGS=-c -Wall -DUSESTB -I./include -I./extra
LDFLAGS=-lm

# TODO: Remove this one things stabilize
BUILD=debug

SRC_DIR := src
OBJ_DIR := obj

OBJECTS=$(SOURCES:.c=.o)
TEST1=test1/app
MDL_TEST=mdl-test/test
MD2_TEST=md2-test/test
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

endif

ifdef EMSDK
	# TODO needs some work before the Emscripten version will work...
	CC = emcc
	LDFLAGS += -s ASYNCIFY -s ASYNCIFY_IGNORE_INDIRECT
	WEB_DIR = web
	WEB = $(WEB_DIR)/index.html
	ALL = $(WEB)
	OBJ_DIR = wobj
else
	TEST1:=$(TEST1).exe
	MDL_TEST:=$(MDL_TEST).exe

	ALL = $(EXECUTABLE) $(RPGTEST) $(EDITOR) $(PCXER) $(PACK)
	ifeq ($(OS),Windows_NT)
		LDFLAGS = -lgdi32
		EXECUTABLE := $(EXECUTABLE).exe
		EDITOR := $(EDITOR).exe
		RPGTEST := $(RPGTEST).exe
		PCXER := $(PCXER).exe
		PACK := $(PACK).exe
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Darwin)
			LDFLAGS = -framework Cocoa
		else
			LDFLAGS = -lX11 -lm
		endif
	endif
endif

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIB) $(TEST1) $(MDL_TEST) $(MD2_TEST)

lib: $(LIB)

debug:
	$(MAKE) BUILD=debug

$(TEST1): test1/main.o $(LIB)
	@echo $@
	@$(CC) $^ $(LDFLAGS) -o $@

$(MDL_TEST): mdl-test/mdl-test.o framewrk/game.o $(LIB)
	@echo $@
	@$(CC) $^ $(LDFLAGS) -o $@

$(MD2_TEST): md2-test/md2-test.o framewrk/game.o $(LIB)
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

mdl-test/%.o: mdl-test/%.c
	@echo $@
	@$(CC) $(CFLAGS) -I ./framewrk -c $< -o $@

md2-test/%.o: md2-test/%.c
	@echo $@
	@$(CC) $(CFLAGS) -I ./framewrk -c $< -o $@

framewrk/%.o: framewrk/%.c
	@echo $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@echo $@
	@mkdir -p $@

# Add header dependencies here

$(OBJ_DIR)/bm_cache.o: src/bm_cache.c extra/bmph.h include/fx.h include/bm_cache.h
$(OBJ_DIR)/fx.o: src/fx.c extra/glmatrix.h extra/bmph.h extra/stb_image.h \
 include/fx.h
$(OBJ_DIR)/md2.o: src/md2.c include/md2.h include/fx.h
$(OBJ_DIR)/md5.o: src/md5.c extra/glmatrix.h include/md5.h extra/bmph.h include/fx.h \
 include/bm_cache.h
$(OBJ_DIR)/mdl.o: src/mdl.c extra/bmph.h include/mdl.h include/fx.h
$(OBJ_DIR)/obj.o: src/obj.c include/fx.h include/obj.h
$(OBJ_DIR)/shapes.o: src/shapes.c include/fx.h include/shapes.h extra/par_shapes.h
test1/main.o: test1/main.c extra/bmph.h include/fx.h extra/glmatrix.h
framewrk/game.o: framewrk/game.c framewrk/fenster.h framewrk/game.h extra/bmph.h
mdl-test/mdl-test.o: mdl-test/mdl-test.c extra/bmph.h framewrk/game.h \
 extra/glmatrix.h include/fx.h include/mdl.h framewrk/fenster.h
md2-test/md2-test.o: md2-test/md2-test.c extra/bmph.h framewrk/game.h \
 extra/glmatrix.h include/fx.h include/md2.h framewrk/fenster.h


# Hide warnings in stb_image.h
$(OBJ_DIR)/fx.o: CFLAGS+=-Wno-unused-but-set-variable

.PHONY : clean dist

dist: dist.zip
dist.zip: clean
	@zip -r $@ *

deps:
	@$(CC) -MM -I ./include -DUSESTB -I ./extra $(SRC) |  sed 's/\(.*\.o:\)/$$(OBJ_DIR)\/\1/'
	@$(CC) -MM -I ./include -I ./extra test1/*.c |  sed 's/\(.*\.o:\)/test1\/\1/'
	@$(CC) -MM -I ./include -I ./extra -I ./framewrk framewrk/*.c |  sed 's/\(.*\.o:\)/framewrk\/\1/'
	@$(CC) -MM -I ./include -I ./extra -I ./framewrk mdl-test/*.c |  sed 's/\(.*\.o:\)/mdl-test\/\1/'
	@$(CC) -MM -I ./include -I ./extra -I ./framewrk md2-test/*.c |  sed 's/\(.*\.o:\)/md2-test\/\1/'

clean:
	@echo Cleaning...
	@-rm -f $(LIB)
	@-rm -rf $(OBJ_DIR) $(WEB_DIR)
	@-rm -f $(TEST1) test1/*.o
	@-rm -f $(MDL_TEST) mdl-test/*.o
	@-rm -f $(MD2_TEST) md2-test/*.o
	@-rm -f framewrk/*.o
	@-rm -f out.gif pick.gif
	@-rm -f dist.zip
