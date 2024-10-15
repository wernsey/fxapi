#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "bmph.h"
#include "game.h"

#include "glmatrix.h"
#include "fx.h"

#include "mdl.h"

#define FENSTER_HEADER
#include "fenster.h"

//static Bitmap *screen = NULL;
static Bitmap *pick_buffer = NULL;

MDL_MESH *mdl = NULL;

static FILE *logfile = NULL;

static void rerror(const char *fmt, ...) {
	FILE *f = logfile;
	if(!f) f = stderr;
    va_list arg;
    va_start(arg, fmt);
    fputs("ERROR: ", f);
    vfprintf(f, fmt, arg);
    fputc('\n', f);
    va_end(arg);
    fflush(f);
}

int init_thing(int argc, char *argv[]) {

	fx_error = rerror;

	if(argc > 1) {
		/* You need to set the palette before loading the model */
		if(argc > 2 && !strcmp(argv[2], "hexen2")) {
			info("using hexen2 palette");
			mdl_set_palette(mdl_hexen2_palette);
		}
		mdl = mdl_load(argv[1]);
	} else {
		error("Usage: %s file.mdl [hexen2]", argv[0]);
		return 0;
	}
	if(!mdl) {
		error("couldn't load MDL file '%s'", argv[1]);
		return 0;
	}

	fx_set_viewport(screen);

	/* The pick buffer is just a second bitmap that will be colored
	in as the renderer draws pixels.
	You need to manage its memory yourself. */
	pick_buffer = bm_create(bm_width(screen), bm_height(screen));
	if(!pick_buffer) return 0;
	fx_set_pick(pick_buffer);

	/* The fog effect is a simple linear fog with a color
		and near/far parameters */
	fx_fog(FX_FOG_LINEAR);
	fx_fog_params(0.5, 0.5, 0.5, 0.9, 1.0, 2);

	/* You can enable/disable backface drawing */
	//fx_backface(1);

	/* The blend mode uses a bit shifting trick to blend */
	// fx_blend(1);



	/* To use transparent pixels on the texture map, set
		the color of the texture, and enable transparency */
	// fx_transparent(1);

	double projection[16];
	double ratio = (double)bm_width(screen) / bm_height(screen);
	mat4_perspective(60.0, ratio, 0.2, 10.0, projection);
	fx_set_projection(projection);

	return 1;
}

void deinit_thing() {
	fx_cleanup();
	bm_free(pick_buffer);
	if(mdl) {
		mdl_free(mdl);
	}
}

double pos[] = {0, -0.5, 0};

void update_screen(int64_t ms) {

	double elapsedSeconds = (double)ms / 1000.0;

	bm_set_color(screen, 0x0000055);
	bm_clear(screen);
	bm_set_color(screen, 0xFF0000);
	bm_printf(screen, 10, 10, "Hello World");

	bm_set_color(pick_buffer, 0x000000);
	bm_clear(pick_buffer);
	bm_set_color(pick_buffer, 0x00FF00);

	fx_clear_zbuf();

	double view[16];
	double eyePos[] = {0,0,1},
		center[] = {0,0,0},
		up[] = {0, 1, 0};
	mat4_lookAt(eyePos, center, up, view);
	fx_set_view(view);

	double model[16];
	static double theta = 0.0;

	static int pause = 0;
	if(!pause)
		theta += 0.01 * elapsedSeconds * 360 / M_PI;
	if(key_pressed('P')) {
		pause = !pause;
	}

	double step = 0.2;
	if(key_pressed(KEY_LEFT)) {
		pos[0] += step;
	}
	if(key_pressed(KEY_RIGHT)) {
		pos[0] -= step;
	}
	if(key_pressed(KEY_UP)) {
		pos[2] += step;
	}
	if(key_pressed(KEY_DOWN)) {
		pos[2] -= step;
	}
	if(key_pressed('Q')) {
		pos[1] += step;
	}
	if(key_pressed('Z')) {
		pos[1] -= step;
	}
	if(key_pressed('R')) {
		pos[0] = 0; pos[1] = 0; pos[2] = 0;
		theta = 0.0;
	}

	mat4_identity(model);
	mat4_translate(model, pos, NULL);
	mat4_rotateY(model, theta, NULL);

	fx_set_model(model);

	// fx_fog(0);
	fx_all_lighting(1);
	fx_set_ambient(0.5, 0.5, 0.5);

	fx_light_enable(0);
	fx_set_diffuse_direction(0, -0.5, -1.0, -0.5);
	fx_set_diffuse_color(0, 1, 1, 1);

	static double current_frame = 0;
	current_frame += 5 * elapsedSeconds;

	if(current_frame >= mdl->total_frames)
		current_frame = 0;

    mdl_draw(mdl, current_frame);

	if(key_pressed(KEY_ESCAPE)) {
		quit = 1;
	}

	if(key_pressed('U')) {
		bm_save(pick_buffer, "pick.gif");
	}
}