/**
 *
 * [fenster]: https://github.com/zserge/fenster
 * [cushion]: https://damieng.com/typography/zx-origins/cushion/
 * [oxygen-16]: https://lospec.com/palette-list/oxygen-16
 * [tomthumb]: https://robey.lag.net/2010/01/23/tiny-monospace-font.html */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

/* I should note that the fenster.h here
has some customizations for Windows/GDI that
the official version doesn't have
(It centers the window, ensures the client
area has the correct size and prevents resizing
the window)
I should perhaps send a pull request if I get a
chance.

TODO: My old GDI code has this in the WndProc as
well that I should add

```
// Don't erase the background - it causes flickering
// http://stackoverflow.com/a/14153470/115589
case WM_ERASEBKGND:
	return 1;
```

*/
#include "fenster.h"

#include "game.h"

#include "bmph.h"

Bitmap *screen = NULL;

static FILE *outfile;

struct fenster *F;

int quit = 0;

void error(const char *fmt, ...) {
	char buffer[128];
	va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, arg);
    va_end(arg);
    fprintf(outfile, "error: %s\n", buffer);
	fflush(outfile);
}

void info(const char *fmt, ...) {
	char buffer[128];
	va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, arg);
    va_end(arg);
    fprintf(outfile, "info: %s\n", buffer);
	fflush(outfile);
}

void debug(const char *fmt, ...) {
#if !defined(NDEBUG)
	char buffer[128];
	va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, arg);
    va_end(arg);
    fprintf(outfile, "debug: %s\n", buffer);
	fflush(outfile);
#else
	(void)fmt;
#endif
}

/* 16-bit maximal LFSR that I found here:
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
 * https://en.wikipedia.org/wiki/Linear-feedback_shift_register
 * See also https://fabiensanglard.net/fizzlefade/, where I got the original idea.
 */
uint16_t sequence(uint16_t *seed) {
	uint16_t lfsr = *seed;
	lfsr ^= lfsr >> 7;
	lfsr ^= lfsr << 9;
	lfsr ^= lfsr >> 13;
	*seed = lfsr;
	return lfsr;
}

// https://en.wikipedia.org/wiki/Smoothstep
double smoothstep(double x) {
	if(x <= 0.0) return 0.0;
	if(x >= 1.0) return 1.0;
	return (3.0 - 2.0 * x) * x * x;
}

double smootherstep(double x) {
	if(x <= 0.0) return 0.0;
	if(x >= 1.0) return 1.0;
	return  ((6.0 * x - 15.0) * x + 10) * x * x * x;  //6x^5 - 15x^4 + 10x^3
}

/* Copies the pixels in `screen` to fenster's buffer, scaling them as necessary */
static void copy_screen(struct fenster *f) {
	int v = 0;
	int cy = 0;
	for(int y = 0; y < f->height; y++) {

		int u = 0;
		int cx = 0;
		for(int x = 0; x < f->width; x++) {
			fenster_pixel(f, x, y) = bm_get(screen, u,v);

			cx = cx + WIDTH;
			while(cx > f->width) {
				u = u + 1;
				cx = cx - f->width;
			}
		}

		cy = cy + HEIGHT;
		while(cy > f->height) {
			v = v + 1;
			cy = cy - f->height;
		}
	}
}

int key_pressed(int key) {
	int mask = 0;
	if(key & 0xFF00) {
		mask = key >> 8;
		if(!(F->mod & mask)) return 0;
		key &= 0xFF;
	}
	if(!mask && F->mod) return 0;
	if(key < 0 || key >= 256) return 0;
	int pressed = F->keys[key];
	F->keys[key] = 0;
	return pressed;
}

static uint32_t buf[VWIDTH * VHEIGHT];

int main(int argc, char *argv[]) {

	struct fenster f = {
		.title = "FXAPI Demo",
		.width = VWIDTH,
		.height = VHEIGHT,
		.buf = buf
	};

	F = &f;

	memset(buf, 0, sizeof buf);

	screen = bm_create(WIDTH, HEIGHT);

#if defined(_WIN32)
	outfile = fopen("fxapi.log", "w");
#else
	outfile = stdout;
#endif

	fenster_open(&f);

	if(!init_thing(argc, argv)) return 1;

	int64_t deltat = 0ll;
	while(fenster_loop(&f) == 0 && !quit) {
		int64_t start = fenster_time();
		update_screen(deltat);
		copy_screen(&f);

#if !defined(__EMSCRIPTEN__) && 0
		if(f.keys[KEY_ESCAPE]) break;
#endif

		/* I expected deltat to be more or less 33 at the end of this section
		 * but that does not seem to be the case on WIN32...
		 */
		int64_t time = fenster_time() - start;
		//info(">>> start: %lld; time: %lld; sleep: %lld", start, time, (1000ll / 30) - time);
		if(time < 1000 / 30)
			fenster_sleep((1000 / 30) - time);
		deltat = fenster_time() - start;
		//info("<<< start: %lld; time: %lld;", start, deltat);

		//static int ctr = 0;
		//if(ctr++ == 30) break;
	}

	deinit_thing();

	fenster_close(&f);

	if(outfile != stdout && outfile != stderr)
		fclose(outfile);

	return 0;
}

char *readfile(const char *fname) {
    FILE *f;
    long len,r;
    char *str;

    if(!(f = fopen(fname, "rb")))
        return NULL;

    fseek(f, 0, SEEK_END);
    len = (long)ftell(f);
    rewind(f);

    if(!(str = malloc(len+2)))
        return NULL;
    r = fread(str, 1, len, f);

    if(r != len) {
        free(str);
        return NULL;
    }

    fclose(f);
    str[len] = '\0';
    return str;
}
