/**
 * @src/menustate.h
 */
#include <GFraMe/GFraMe_accumulator.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_tilemap.h>
#include <GFraMe/GFraMe_util.h>
#include "background.h"
#include "global.h"
#include "score.h"
#include "menustate.h"

// Define some variables needed by the events module
GFraMe_event_setup();

enum {
	ENTER,
	GOTO_LOOP,
	LOOP,
	GOTO_EXIT,
	EXIT
};
static int state;

static int game_init;

static int time;
static GFraMe_accumulator timer;

static GFraMe_tilemap init_text;
static char init_data[20];
static int is_text_visible;

#define BUG_X	110
#define BUG_Y	29
#define SQUASHER_X	16
#define SQUASHER_Y	65
#define VDIST	16
#define HDIST	37
enum {
	B=0,u,g, MAX_BUG
};
enum {
	S=0,q,u2,a,s,h,e,r,
	MAX_SQUASHER
};
static GFraMe_sprite BUG[MAX_BUG];
static GFraMe_sprite SQUASHER[MAX_SQUASHER];

static void menu_init();
static void menu_event();
static void menu_update();
static void menu_draw();

void ms_loop() {
	menu_init();
	while (game_init == 0 && gl_running) {
		menu_event();
		switch (state) {
			case ENTER:
				time += GFraMe_event_elapsed;
				if (time >= 1500)
					state = GOTO_LOOP;
				else {
					menu_update();
					is_text_visible = 0;
				}
			break;
			case GOTO_LOOP:
				#define SPR_POS(TYPE, LETTER) \
					GFraMe_object_set_pos(&TYPE[LETTER].obj, \
										  TYPE##_X + HDIST * LETTER, \
										  TYPE##_Y + VDIST*(LETTER != 0));\
					TYPE[LETTER].obj.vy = 0
				SPR_POS(BUG, B);
				SPR_POS(BUG, u);
				SPR_POS(BUG, g);
				SPR_POS(SQUASHER, S);
				SPR_POS(SQUASHER, q);
				SPR_POS(SQUASHER, u2);
				SPR_POS(SQUASHER, a);
				SPR_POS(SQUASHER, s);
				SPR_POS(SQUASHER, h);
				SPR_POS(SQUASHER, e);
				SPR_POS(SQUASHER, r);
				#undef SPR_POS
				state = LOOP;
			break;
			case LOOP: menu_update(); break;
		}
		menu_draw();
	}
}

static void menu_init() {
	int i;
	i = 0;
	game_init = 0;
	time = 0;
	// Initialize the background
	background_init();
	// Set init text
	is_text_visible = 1;
	GFraMe_tilemap_init(&init_text, 20, 1, init_data, &gl_sset8, NULL, 0);
	init_text.x = (320 - 20*8) / 2;
	init_text.y = 240 - 32 - 12;
	i = 0;
	init_data[i++] = CHAR2TILE('-');
	init_data[i++] = CHAR2TILE('-');
	init_data[i++] = CHAR2TILE(' ');
#ifndef MOBILE
	init_data[i++] = CHAR2TILE('C');
	init_data[i++] = CHAR2TILE('L');
	init_data[i++] = CHAR2TILE('I');
	init_data[i++] = CHAR2TILE('C');
	init_data[i++] = CHAR2TILE('K');
#else
	init_data[i++] = CHAR2TILE('T');
	init_data[i++] = CHAR2TILE('O');
	init_data[i++] = CHAR2TILE('U');
	init_data[i++] = CHAR2TILE('C');
	init_data[i++] = CHAR2TILE('H');
#endif
	init_data[i++] = CHAR2TILE(' ');
	init_data[i++] = CHAR2TILE('T');
	init_data[i++] = CHAR2TILE('O');
	init_data[i++] = CHAR2TILE(' ');
	init_data[i++] = CHAR2TILE('S');
	init_data[i++] = CHAR2TILE('T');
	init_data[i++] = CHAR2TILE('A');
	init_data[i++] = CHAR2TILE('R');
	init_data[i++] = CHAR2TILE('T');
	init_data[i++] = CHAR2TILE(' ');
	init_data[i++] = CHAR2TILE('-');
	init_data[i++] = CHAR2TILE('-');
	// Initialize the title
	#define INIT_SPR(TYPE, LETTER, H, SSET, TILE) \
		GFraMe_sprite_init(TYPE + LETTER, TYPE##_X + HDIST*LETTER, \
						   -H, 32, H, \
						   &gl_sset##SSET, 0, 0); \
		TYPE[LETTER].cur_tile = TILE; \
		TYPE[LETTER].obj.vy = H + TYPE##_Y + VDIST*(LETTER != 0); \
		if (TYPE == SQUASHER) { \
			GFraMe_object_set_y(&TYPE[LETTER].obj, \
								(int)(TYPE[LETTER].obj.y \
									  - TYPE[LETTER].obj.vy)); \
			TYPE[LETTER].obj.vy *= 2; \
		}
	INIT_SPR(BUG, B, 64, 32x64, 8*3);
	INIT_SPR(BUG, u, 32, 32, 8*5);
	INIT_SPR(BUG, g, 32, 32, 8*5+1);
	INIT_SPR(SQUASHER, S, 64, 32x64, 8*3+1);
	INIT_SPR(SQUASHER, u2, 32, 32, 8*5);
	INIT_SPR(SQUASHER, q, 32, 32, 8*5+2);
	INIT_SPR(SQUASHER, a, 32, 32, 8*5+3);
	INIT_SPR(SQUASHER, s, 32, 32, 8*5+4);
	INIT_SPR(SQUASHER, h, 32, 32, 8*5+5);
	INIT_SPR(SQUASHER, e, 32, 32, 8*5+6);
	INIT_SPR(SQUASHER, r, 32, 32, 8*5+7);
	#undef INIT_SPR
	// Initialize the timer and clean the events accumulated on the queue
	GFraMe_accumulator_init_time(&timer, 500, 900);
	GFraMe_event_init(60, 60);
}

static void menu_event() {
	GFraMe_event_begin();
		GFraMe_event_on_timer();
		GFraMe_event_on_mouse_down();
			switch (state) {
				case ENTER: state = GOTO_LOOP; break;
				case LOOP: game_init = 1; break;
			}
		GFraMe_event_on_quit();
			GFraMe_log("Received quit!");
			gl_running = 0;
	GFraMe_event_end();
}
static void menu_update() {
	GFraMe_event_update_begin();
		int i;
		GFraMe_accumulator_update(&timer, GFraMe_event_elapsed);
		if (GFraMe_accumulator_loop(&timer)) {
			is_text_visible = !is_text_visible;
		}
		i = 0;
		while (i < MAX_BUG) {
			GFraMe_sprite_update(BUG + i, GFraMe_event_elapsed);
			i++;
		}
		i = 0;
		while (i < MAX_SQUASHER) {
			GFraMe_sprite_update(SQUASHER + i, GFraMe_event_elapsed);
			i++;
		}
	GFraMe_event_update_end();
}
static void menu_draw() {
	GFraMe_event_draw_begin();
		int i;
		// Draw the background
		background_draw();
		// Draw start message
		if (is_text_visible)
			GFraMe_tilemap_draw(&init_text);
		i = 0;
		while (i < MAX_BUG) {
			GFraMe_sprite_draw(BUG + i);
			i++;
		}
		i = 0;
		while (i < MAX_SQUASHER) {
			GFraMe_sprite_draw(SQUASHER + i);
			i++;
		}
	GFraMe_event_draw_end();
}
