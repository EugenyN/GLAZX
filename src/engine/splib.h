// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

#define SCREEN_UPDATE	sp1_UpdateNow()

#ifndef NULL
#define NULL    ((uchar *)0)
#endif

void splib_init()
{
	sp1_Initialize(SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE, INK_BLACK | PAPER_WHITE, ' ');

	// load tileset
	gp_gen = tileset;
	for (it = 0; it != TILES_COUNT; ++it, gp_gen += 8)
		sp1_TileEntry(it, gp_gen);
}

void clear_game_area()
{
	sp1_ClearRect(&game_area_rect, 0, 0, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
	sp1_Invalidate(&game_area_rect);
}

void clear_full_screen()
{
	sp1_ClearRect(&fullscreen_rect, 0, 0, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
	sp1_Invalidate(&fullscreen_rect);
}

void print_tile() 
{
	gp_aux = tileset_map + (_t << 3);
	sp1_PrintAtInv(_y	 , _x	 , *gp_aux ++, *gp_aux ++);
	sp1_PrintAtInv(_y	 , _x + 1, *gp_aux ++, *gp_aux ++);
	sp1_PrintAtInv(_y + 1, _x	 , *gp_aux ++, *gp_aux ++);
	sp1_PrintAtInv(_y + 1, _x + 1, *gp_aux ++, *gp_aux );
}

void print_number1(unsigned char x, unsigned char y, unsigned char c, unsigned char n) 
{
	sp1_PrintAtInv(y, x, c, DIGIT(n));
}

void print_number2(unsigned char x, unsigned char y, unsigned char c, unsigned char n) 
{
	sp1_PrintAtInv(y, x, c, DIGIT(n / 10));
	sp1_PrintAtInv(y, x + 1, c, DIGIT(n % 10));
}

void print_str(unsigned char x, unsigned char y, unsigned char c, unsigned char *s) 
{
	//TODO: see SP1 sp1_PrintString / sp1_SetPrintPos
	while (*s) sp1_PrintAtInv(y, x ++, c, (*s ++) - 32);
}

void sprites_init()
{
	rda = 0;
	it = SW_SPRITES_16x16; while (it--) {
		spr_sw[rda] = sp1_CreateSpr(SP1_DRAW_MASK2NR, SP1_TYPE_2BYTE, 3, 16, rda);
		sp1_AddColSpr(spr_sw[rda], SP1_DRAW_MASK2NR, 0, 64, rda);
		sp1_AddColSpr(spr_sw[rda], SP1_DRAW_MASK2NR, 0, 0, rda);
		rda++;
	}
}

void sprites_update()
{
	it = SW_SPRITES_ALL; while (it--) 
	{
		if (spr_on[it])
			sp1_MoveSprAbs(spr_sw[it], &game_area_rect,	spr_frame[it], SCR_Y + (spr_y[it] >> 3), SCR_X + (spr_x[it] >> 3), 0, 0);
		else
			sp1_MoveSprAbs(spr_sw[it], &game_area_rect, NULL, 0, 34, 0, 0);
	}
}

void sprites_hide_all() 
{
	rda = 0;
	it = SW_SPRITES_ALL; while (it-- > rda) { 
		sp1_MoveSprAbs(spr_sw[it], &game_area_rect, NULL, 0, 34, 0, 0);
	}
}

void add_colour_callback(struct sp1_cs *cs)
{
	// color chars 0,1,3,4
	if (rda == 0 || rda == 1 || rda == 3 || rda == 4) {
		cs->attr = rdb;
		cs->attr_mask = 0;
	}
	rda++;
}

void sprite_set_color(unsigned char idx, unsigned char color)
{
	rda = 0;
	rdb = color;
	sp1_IterateSprChar(spr_sw[idx], add_colour_callback);
}