#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <unistd.h>

#include "bmph.h"
#include "game.h"

#include "glmatrix.h"
#include "fx.h"

#include "md5.h"

#define FENSTER_HEADER
#include "fenster.h"

static Bitmap *pick_buffer = NULL;

static MD5_MODEL *md5 = NULL;
static MD5_ANIM *anim = NULL;

double scalar = 1;

void load_shader(char *shader_name, char * file) {
	Bitmap *texture = bm_load(file);
	if(!texture) {
		error("Unable to load texture %s for '%s'", file, shader_name);
		return;
	}
	info("loaded texture %s for '%s'", file, shader_name);
	md5_set_shader(shader_name, texture);
}

int init_thing(int argc, char *argv[]) {

	fx_error = error;

#if 1
	if(argc < 4) {
		error("Usage: %s directory mesh.md5mesh anim.md5anim", argv[0]);
		return 0;
	}

	const char *dir = argv[1];
	const char *meshname = argv[2];
	const char *animname = argv[3];

	chdir(dir);
	md5 = md5_load_mesh(meshname);
	if(!md5) {
		error("couldn't load MD5 file %s", meshname);
		return 0;
	}
	info("md5 model loaded");
	anim = md5_load_anim(animname);
	if(!anim) {
		error("couldn't load MD5 animation %s", animname);
		return 0;
	}
	info("md5 animation loaded");

	for(int i = 0; i < md5->numMeshes; i++) {
		const char *name = md5->meshes[i].shader;
		Bitmap *texture = bm_loadf("%s.tga", name);
		if(!texture) {
			error("Unable to load texture for '%s'", name);
			continue;
		}
		md5_set_shader(name, texture);
	}
	info("md5 textures loaded");
#elif 0
	md5 = md5_load_mesh("md5-test/md5/bob_lamp_update/bob_lamp_update.md5mesh");
	if(!md5) {
		error("couldn't load MD5 mesh");
		return 0;
	}
	info("md5 model loaded");
	anim = md5_load_anim("md5-test/md5/bob_lamp_update/bob_lamp_update.md5anim");
	if(!anim) {
		error("couldn't load MD5 animation");
		return 0;
	}
	info("md5 animation loaded");

	int i;
	for(i = 0; i < md5->numMeshes; i++) {
		const char *name = md5->meshes[i].shader;
		Bitmap *texture = bm_loadf("md5-test/md5/bob_lamp_update/%s.tga", name);
		if(!texture) {
			error("Unable to load texture for '%s'", name);
			continue;
		}
		md5_set_shader(name, texture);
	}
	info("md5 textures loaded");
#elif 0
	md5 = md5_load_mesh("md5-test/md5/player/player.md5mesh");
	if(!md5) {
		error("couldn't load MD5 mesh");
		return 0;
	}
	info("md5 model loaded");
	load_shader("models/characters/player/playerhead", "md5-test/md5/player/textures/playerhead.tga");
	load_shader("models/characters/player/teethplayer", "md5-test/md5/player/textures/teeth.tga");
	load_shader("models/characters/common/leftbrown", "md5-test/md5/player/textures/brown.tga");
	load_shader("models/characters/common/rightbrown", "md5-test/md5/player/textures/brown.tga");
	load_shader("models/characters/player/body", "md5-test/md5/player/textures/body.tga");
	load_shader("models/characters/player/body2", "md5-test/md5/player/textures/body.tga");
	load_shader("models/characters/player/arm2", "md5-test/md5/player/textures/arm2.tga");
	scalar = 0.01;
#elif 0
	md5 = md5_load_mesh("md5-test/md5/hellknight/hellknight.md5mesh");
	if(!md5){
		error("couldn't load MD5 mesh");
		return 0;
	}
	info("md5 model loaded");
	load_shader("models/monsters/hellknight/hellknight", "md5-test/md5/hellknight/hellknight_diffuse.jpg");
	load_shader("models/monsters/hellknight/gob", "md5-test/md5/hellknight/gob.png");
	anim = md5_load_anim("md5-test/md5/hellknight/idle2.md5anim");
	if(!anim) {
		error("couldn't load MD5 animation");
		return 0;
	}
	info("md5 animation loaded");
	scalar = 0.01;
#elif 0
	md5 = md5_load_mesh("md5-test/md5/zfat/zfat.md5mesh");
	if(!md5) {
		error("couldn't load MD5 mesh");
		return 0;
	}
	info("md5 model loaded");
	// anim = md5_load_anim("md5-test/md5/zfat/idle1.md5anim");
	anim = md5_load_anim("md5-test/md5/zfat/attack2.md5anim");
	if(!anim) {
		error("couldn't load MD5 animation");
		return 0;
	}
	info("md5 animation loaded");
	load_shader("models/monsters/zombie/fatty/fatty", "md5-test/md5/zfat/textures/fatty_d.tga");
	scalar = 0.01;
#endif

	fx_set_viewport(screen);

	/* The pick buffer is just a second bitmap that will be colored
	in as the renderer draws pixels.
	You need to manage its memory yourself. */
	pick_buffer = bm_create(bm_width(screen), bm_height(screen));
	if(!pick_buffer) {
		error("couldn't create pick buffer");
		return 0;
	}
	fx_set_pick(pick_buffer);

	/* The fog effect is a simple linear fog with a color
		and near/far parameters */
	//fx_fog(FX_FOG_LINEAR);
	fx_fog_params(0, 0, 0, 0.5, 1.0, 2);

	/* You can enable/disable backface drawing */
	//fx_backface(1);

	/* The blend mode uses a bit shifting trick to blend */
	// fx_blend(1);

	double projection[16];
	double ratio = (double)bm_width(screen) / bm_height(screen);
	mat4_perspective(60.0, ratio, 0.02, 100.0, projection);
	fx_set_projection(projection);

	return 1;
}

void deinit_thing() {
	md5_free_anim(anim);
	md5_free(md5);
	fx_cleanup();
	bm_free(pick_buffer);
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
	mat4_scale_scalar(model, scalar, NULL);

	fx_set_model(model);

	fx_all_lighting(1);
	fx_set_ambient(0, 0, 0);

	fx_light_enable(0);
	fx_set_diffuse_direction(0, -0.5, -1.0, -0.5);
	fx_set_diffuse_color(0, 1, 1, 1);

	static double current_frame = 0;

	fx_set_target_color(0x00FF00);
	if(anim) {
		current_frame += elapsedSeconds * anim->frameRate;
		if(current_frame > anim->numFrames)
			current_frame = 0;
		md5_draw_frame(md5, anim, current_frame);
	} else
		md5_draw(md5);

	if(key_pressed(KEY_ESCAPE)) {
		quit = 1;
	}

	if(key_pressed('U')) {
		bm_save(pick_buffer, "pick.gif");
	}
}