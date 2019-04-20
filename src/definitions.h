// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

// Memory layout

#define MALLOC_ADDR				0xD000
#ifdef MODE_128K
	#define FREE_BUFFER			0xD101 // 0xD101 = MALLOC_ADDR + 257
#else
	// we can use printer buffer 0x5B00-0x5C00 on 48K Spectrum
	#define FREE_BUFFER			0x5B00
#endif

// Pixels and numbers

#define TL(px)					((px) >> 4)
#define PX(tl)					((tl) << 4)
#define MAX(x1,x2)				((x1) < (x2) ? (x2) : (x1))
#define MIN(x1,x2)				((x1) < (x2) ? (x1) : (x2))
#define DIGIT(n)				(16 + (n))

// Sprite frames in spriteset resources

#define MAX_FRAMES_16x16		16
#define MAX_FRAMES				MAX_FRAMES_16x16

#define ATTACK_FRAME_BASE		8
#define EXPLOSION_FRAME_BASE	12
#define BULLET_FRAME_BASE		14

// Software sprites instances

#define SW_SPRITES_16x16		6 // 4 bullets + 2 glazes
#define SW_SPRITES_ALL			6
#define SW_SPRITES_BLOCKS		(SW_SPRITES_16x16 * 10) // 1 + Row * Columns blocks for each sprite. e.g 16x16 sprite needs 1+3*3 = 10 blocks
#define SW_SPRITES_MEMORY		(SW_SPRITES_BLOCKS * 25) // 25 bytes (+1 overhead)

// Sprite bases in array

#define SPR_PLAYER_BASE			0
#define SPR_BULLET_BASE			2

// Screen area

#define SCR_X					1
#define SCR_Y					2
#define SCR_BUFFER_SIZE			150

// Input

#define INPUT_READ(p)					((joyfunc[p]) (&keys[p]))
#define INPUT_LEFT(a)					((a) & in_LEFT)
#define INPUT_RIGHT(a)					((a) & in_RIGHT)
#define INPUT_UP(a)						((a) & in_UP)
#define INPUT_DOWN(a)					((a) & in_DOWN)
#define INPUT_FIRE(a)					((a) & in_FIRE)

#define INPUT_LEFT_PRESSED(pad, last)	(INPUT_LEFT(pad) && !INPUT_LEFT(last))
#define INPUT_RIGHT_PRESSED(pad, last)	(INPUT_RIGHT(pad) && !INPUT_RIGHT(last))
#define INPUT_UP_PRESSED(pad, last)		(INPUT_UP(pad) && !INPUT_UP(last))
#define INPUT_DOWN_PRESSED(pad, last)	(INPUT_DOWN(pad) && !INPUT_DOWN(last))
#define INPUT_FIRE_PRESSED(pad, last)	(INPUT_FIRE(pad) && !INPUT_FIRE(last))

#define PAUSE_KEY				0x087f
#define EXIT_KEY				0x047f

#define CTRL_KEYBOARD1			1
#define CTRL_KEYBOARD2			2
#define CTRL_KEMPSTON			3
#define CTRL_SINCLAIR1			4
#define CTRL_SINCLAIR2			5

// Hud

#define GLAZ_SYMBOL				"^"
#define GLAZ_SYMBOL2			"_"
#define BULLET_SYMBOL			"*"
#define STAT_X					6
#define STAT_Y					0
#define TIMER_X					12
#define TIMER_Y					23

// Glazes

#define FACING_RIGHT			0
#define FACING_LEFT				1
#define FACING_UP				2
#define FACING_DOWN				3

#define IDLE_ANIMATION_TIME		8
#define ATTACK_ANIMATION_TIME	10
#define EXPL_ANIMATION_TIME		8

#define STATE_IDLE				0
#define STATE_ATTACK			1
#define STATE_EXPLODES			2

#define GHOST_MODE				1
#define SUPER_GHOST_MODE		2
#define SUPER_GHOST_MODE_TIME	200
#define ROCKET_AMMO_REFILL		1

// Bullets

#define BULLET_SPEED			1
#define MAX_BULLETS				4  // max number of bullets on screen.
#define MAX_BULLETS_PER_GLAZ	2

// Game

#define MOD_NORMAL				0
#define MOD_PRO					1
#define MOD_NO_GHOST			2
#define MOD_ONE_SHOT			3
#define MODS_COUNT				4

#define MAX_GLAZES				2

#define GAME_END_NONE			0
#define GAME_END_GLAZ_WIN		1
#define GAME_END_TIME_UP		2
#define GAME_END_EXIT			3

#define GAMES_PLAYED_FOR_SECRET	8

#define TIMER_INITIAL			99
#define TIMER_LAPSE				25	// # of frames between decrements

// Maps and tiles

#define MAP_COUNT				16 + 2
#define MAP_WIDTH				15
#define MAP_HEIGHT				10

#define TILES_COUNT				139 // font - 64 chars, background - 75 chars
#define AMMO_TILE				17
#define LIVE_TILE				18
#define GHOST_TILE				19
#define ROCKET_TILE				20
#define BONUS_TILE(t)			((t) >= AMMO_TILE && (t) <= ROCKET_TILE)

#define WALKABLE_TILE_ATTR		0
#define OBSTACLE_TILE_ATTR		8

// Sound

#ifdef MODE_128K
	#define BASE_SOUND			0xC000
	#define SE_WYZ
#else
	#define SE_BEEPER
#endif

//TODO: clear unused
#ifdef SE_WYZ
	#define SFX_SELECT			10
	#define SFX_START			11
	#define SFX_PAUSE			11
	#define SFX_FIRE			6
	#define SFX_NO_AMMO_FIRE	7
	#define SFX_GLAZ_HIT		5
	#define SFX_DEATH			8
	#define SFX_GET_BONUS		2
	#define SFX_BONUS_SPAWN		12
	#define SFX_BONUS_HIT		5
	#define SFX_ROUND_START		14
	#define SFX_ROUND_FINISHED	14

	#define TOTAL_SFX_COUNT		16
#else
	#define SFX_SELECT			0
	#define SFX_START			5
	#define SFX_PAUSE			5
	#define SFX_FIRE			1
	#define SFX_NO_AMMO_FIRE	0
	#define SFX_GLAZ_HIT		6
	#define SFX_DEATH			7
	#define SFX_GET_BONUS		4
	#define SFX_BONUS_SPAWN		3
	#define SFX_BONUS_HIT		2
	#define SFX_ROUND_START		0
	#define SFX_ROUND_FINISHED	8

	#define TOTAL_SFX_COUNT		9
#endif

#define MUSIC_TITLE				0

// Sound engine

#ifdef SE_WYZ
	#define SE_INIT				wyz_init
	#define SE_ISR				wyz_ISR
	#define SFX_PLAY(n)			wyz_play_sound(n)
	#define MUSIC_PLAY(n)		wyz_play_music(n)
	#define MUSIC_STOP			wyz_stop_sound
#endif

#ifdef SE_BEEPER
	#define SE_INIT()			;
	#define SE_ISR()			;
	#define SFX_PLAY(n)			beep_fx(n)
	#define MUSIC_PLAY(n)		beep_play_music()
	#define MUSIC_STOP()		;
#endif

#ifdef SE_NONE
	#define SE_INIT()			;
	#define SE_ISR()			;
	#define SFX_PLAY(n)			;
	#define MUSIC_PLAY(n)		;
	#define MUSIC_STOP()		;
#endif