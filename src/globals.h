// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

// SP1 / sprites

struct sp1_Rect game_area_rect = { SCR_Y, SCR_X, 30, 20 };
struct sp1_Rect fullscreen_rect = { 0, 0, 32, 24 };
struct sp1_ss *spr_sw[SW_SPRITES_ALL];
unsigned char *spr_frames[MAX_FRAMES];
unsigned char *spr_frame[SW_SPRITES_ALL];
unsigned char spr_idx;
unsigned char spr_on[SW_SPRITES_ALL];
unsigned char spr_x[SW_SPRITES_ALL], spr_y[SW_SPRITES_ALL];

// Input

#ifdef Z88DK199C
typedef unsigned int (*JFUNC)();
#else
#define JFUNC void*
#endif
struct in_UDK keys[MAX_GLAZES];
JFUNC joyfunc[MAX_GLAZES];

// IM2

unsigned char frames;

// General shared/reused vars

unsigned char it, jt;
unsigned char rda, rdb, rdc, rdi;
unsigned char rdx, rdy, rdt, rdct;
unsigned char *gp_gen, *gp_aux;
unsigned char _x, _y, _t;

// Map stuff

unsigned char *gp_map;
unsigned char random_map_data[SCR_BUFFER_SIZE];

// Tiles and screen buffers

unsigned char scr_buff[SCR_BUFFER_SIZE] @ FREE_BUFFER;
#ifdef MODE_128K
	unsigned char scr_attr[SCR_BUFFER_SIZE];
#else
	unsigned char scr_attr[SCR_BUFFER_SIZE] @ (FREE_BUFFER + SCR_BUFFER_SIZE);
#endif	

// 0 = walkable 8 = obstacle
unsigned char tile_attr [] = {
	0, 0, 0, 8, 8, 0, 0, 8, 
	8, 0, 0, 8, 8, 0, 0, 8, 
	8, 0, 0, 0, 0, 0, 0, 0
};

// Gameplay

unsigned char do_game;
unsigned char game_end_result;
unsigned char bonus_delay;
unsigned char game_mode;
unsigned char games_played;
unsigned char maps_count;
unsigned char current_map;

typedef struct {
	unsigned char start_lives;
	unsigned char start_ammo;
	unsigned char bonus_spawn_delay;
	unsigned char ghost_time;
	unsigned char ammo_refill;
} GAME_MODE;
GAME_MODE gm_config;

typedef struct {
	unsigned char on;
	unsigned char t;
	unsigned char frames;
	unsigned char count;
	unsigned char zero;
} LEVEL_TIMER;
LEVEL_TIMER timer;

// Glazes and bullets

unsigned char p_state[MAX_GLAZES];
unsigned char p_state_frame_counter[MAX_GLAZES];
unsigned char p_ghost_mode[MAX_GLAZES];
unsigned char p_ghost_mode_counter[MAX_GLAZES];
unsigned char p_facing[MAX_GLAZES];
unsigned char p_life[MAX_GLAZES];
unsigned char p_ammo[MAX_GLAZES];
unsigned char p_rocket_sight_counter[MAX_GLAZES];
unsigned char p_idle_frame, p_idle_frame_counter;

unsigned char bullet_dir[MAX_BULLETS];
unsigned char bullet_speed[MAX_BULLETS];

unsigned char pad_current[MAX_GLAZES], pad_last[MAX_GLAZES];