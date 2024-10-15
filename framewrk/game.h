#ifndef GAME_H
#define GAME_H

#ifdef FENSTER_H
extern struct fenster *F;
#endif

#include <stdint.h>

#ifdef BMP_H
extern Bitmap *screen;
#endif

#define WIDTH 320
#define HEIGHT 200

#define VWIDTH 640
#define VHEIGHT 480

extern int quit;

extern int init_thing(int argc, char *argv[]);
extern void deinit_thing(void);
extern void update_screen(int64_t ms);

void error(const char *fmt, ...);
void info(const char *fmt, ...);
void debug(const char *fmt, ...);

uint16_t sequence(uint16_t *seed);

double smoothstep(double x);
double smootherstep(double x);

extern uint8_t shades[16];
void reset_shades(void);
uint8_t shade(uint8_t color, double value, int sx, int sy);

char *readfile(const char *fname);

void screenshot(const char *filename);

#define KEY_MASKED(mask, key) (((mask & 0xFF) << 8)|(key & 0xFF))

int key_pressed(int key);

#endif
