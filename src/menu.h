// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

#define GLAZ_COLOR INK_YELLOW | BRIGHT

#define MAIN_MENU_X1	6
#define MAIN_MENU_X2	20
#define MAIN_MENU_Y		5
#define MAIN_MENU_ROW	7
#define MAIN_MENU_WIDTH	7

#define MODE_MENU_X		10
#define MODE_MENU_Y		5
#define MODE_MENU_ROW	4

void main_menu();

void mode_menu_select(unsigned char item)
{
	print_str(rdx, rdy, 71, " ");
	rdx = MODE_MENU_X - 1;
	rdy = MODE_MENU_Y + item;
	print_str(rdx, rdy,  GLAZ_COLOR, GLAZ_SYMBOL);
	SCREEN_UPDATE;
}

void mode_menu() 
{
	rdx = MODE_MENU_X - 1;
	rdy = MODE_MENU_Y;

	clear_game_area();
	print_str(9,  3, 71, "- SELECT MOD -");
	print_str(10, 5, 71, "NORMAL");
	print_str(10, 6, 71, "PROFESIONAL");
	print_str(10, 7, 71, "NO GHOST");
	print_str(10, 8, 71, "ONE SHOT");

	print_str(2, 18, 71, "PRESS UP,DOWN,FIRE TO SELECT");

	mode_menu_select(game_mode);

	while (1) {
		rda = INPUT_READ(0);

		if(INPUT_FIRE_PRESSED(rda, rdb)) {
			SFX_PLAY(SFX_START);
			break;
		} else if (INPUT_DOWN_PRESSED(rda, rdb)) {
			if (game_mode < MODS_COUNT - 1)
				game_mode++;
			else
				game_mode = 0;
			mode_menu_select(game_mode);
			SFX_PLAY(SFX_SELECT);
		} else if (INPUT_UP_PRESSED(rda, rdb)) {
			if (game_mode > 0)
				game_mode--;
			else
				game_mode = MODS_COUNT - 1;
			mode_menu_select(game_mode);
			SFX_PLAY(SFX_SELECT);
		}

		rdb = rda;
	}

	main_menu();
}

void print_sys_info() 
{
#ifdef MODE_128K
	print_str(3, 21, INK_WHITE, "128K V1.0");
#else
	print_str(3, 21, INK_WHITE, "48K V1.0");
#endif
	if (zx_128mode())
		print_str(15, 21, INK_WHITE, "SYSTEM:ZX128");
	else
		print_str(15, 21, INK_WHITE, "SYSTEM:ZX48");	
}

void sfx_menu() 
{
	MUSIC_STOP();
	clear_game_area();
	print_str(10, 3, 71, "- SFX TEST -");
	print_str(5, 8, 71, "PRESS UP,DOWN TO SELECT");
	print_str(5, 9, 71, "PRESS FIRE TO PLAY");
	print_str(5, 10, 71, "PRESS 0 TO EXIT");

	print_sys_info();
	
	SCREEN_UPDATE;

	jt = 0;
	while (1) {
		print_str(13, 12, 71, "SFX:");
		print_number2(17, 12, INK_WHITE,  jt);
		SCREEN_UPDATE;

		rda = INPUT_READ(0);

		if (INPUT_DOWN_PRESSED(rda, rdb)) {
			if (jt < TOTAL_SFX_COUNT - 1) jt++;
		} else if(INPUT_UP_PRESSED(rda, rdb)) {
			if (jt > 0) jt--;
		} else if (INPUT_FIRE_PRESSED(rda, rdb)) {
			SFX_PLAY(jt);
		} else if(in_Inkey() == '0') {
			SFX_PLAY(SFX_START);
			break;
		}

		rdb = rda;
	}
#ifdef MODE_128K
	MUSIC_PLAY(MUSIC_TITLE);
#endif
	main_menu();
}

void controls_menu() 
{
	clear_game_area();

	print_str(8, 3, 71, "- SET CONTROLS -");

	print_str(10, 9, 71, "1.KEYS WSADE");
	print_str(10, 10, 71, "2.KEYS IKJLO");
	print_str(10, 11, 71, "3.KEMPSTON");
	print_str(10, 12, 71, "4.SINCLAIR 1");
	print_str(10, 13, 71, "5.SINCLAIR 2");

	print_str(3, 18, 71, "PRESS NUMBER KEY TO SELECT");

	for (it = 0; it < MAX_GLAZES; it++)
	{
		print_str (13, 6, 71, "GLAZ");
		print_number1(18, 6, INK_WHITE, it + 1);
		SCREEN_UPDATE;

		while (1) {
			jt = in_Inkey() - '0';
			if (jt >= CTRL_KEYBOARD1 && jt <= CTRL_SINCLAIR2) break;
		}
		controls_setup(it, jt);
		in_WaitForNoKey();
		SFX_PLAY(SFX_SELECT);
	}

	main_menu();
}

void main_menu_select(unsigned char item)
{
	print_str(rdx, rdy, 71, " ");
	print_str(rdx + MAIN_MENU_WIDTH, rdy, 71, " ");
	if (item > MAIN_MENU_ROW - 1) {
		rdx = MAIN_MENU_X2 - 1;
		rdy = MAIN_MENU_Y + item - MAIN_MENU_ROW;
	} else {
		rdx = MAIN_MENU_X1 - 1;
		rdy = MAIN_MENU_Y + item;
	}

	print_str(rdx, rdy, GLAZ_COLOR, GLAZ_SYMBOL);
	print_str(rdx + MAIN_MENU_WIDTH, rdy,  GLAZ_COLOR, GLAZ_SYMBOL2);
	SCREEN_UPDATE;
}

void main_menu()
{
	rdx = MAIN_MENU_X1 - 1;
	rdy = MAIN_MENU_Y;

	clear_game_area();

	print_str(9, 3, 71, "- SELECT MAP -");

	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 0, 71, "SNAIL");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 1, 71, "HOUSE");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 2, 71, "PARK");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 3, 71, "JACK");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 4, 71, "MEDUSA");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 5, 71, "CHAPEL");
	print_str(MAIN_MENU_X1, MAIN_MENU_Y + 6, 71, "ALTAR");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 0, 71, "SKULL");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 1, 71, "SPIDER");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 2, 71, "CASTLE");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 3, 71, "STAKAN");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 4, 71, "GHOST");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 5, 71, "GEN 1");
	print_str(MAIN_MENU_X2, MAIN_MENU_Y + 6, 71, "GEN 2");

	// secret maps
	if (maps_count == MAP_COUNT)
	{
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 5, 71, "MAZE  ");
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 6, 71, "PRISON");
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 7, 71, "ALIEN ");
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 8, 71, "GLAZX ");
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 9, 71, "GEN 1");
		print_str(MAIN_MENU_X2, MAIN_MENU_Y + 10, 71, "GEN 2");
	}

	rdi = maps_count == MAP_COUNT ? 4 : 10;

	print_str(rdi, 13, 71, "1.SELECT MOD");
	print_str(rdi, 14, 71, "2.SET CONTROLS");
	print_str(rdi, 15, 71, "3.SFX TEST  ");

	print_str(2, 18, 71, "PRESS UP,DOWN TO SELECT MAP");
	print_str(10, 20, 71, "0.BEGIN GAME");
	
	main_menu_select(current_map);

	wait_no_button();

	while (1) 
	{
		rdc = in_Inkey();
		rda = INPUT_READ(0);
		
		if (rdc == '1') {
			SFX_PLAY(SFX_START);
			mode_menu();
			break;
		} else if (rdc == '2') {
			SFX_PLAY(SFX_START);
			controls_menu();
			break;
		} else if (rdc == '3') {
			SFX_PLAY(SFX_START);
			sfx_menu();
			break;
		} else if (INPUT_FIRE_PRESSED(rda, rdb) || rdc == '0') {
			SFX_PLAY(SFX_START);
			wait_no_button(); // need for 48k mode with kempston joystic
			break;
		} else if (INPUT_DOWN_PRESSED(rda, rdb)) {
			if (current_map < maps_count - 1)
				current_map++;
			else
				current_map = 0;
			main_menu_select(current_map);
			SFX_PLAY(SFX_SELECT);
		} else if (INPUT_UP_PRESSED(rda, rdb)) {
			if (current_map > 0)
				current_map--;
			else
				current_map = maps_count - 1;
			main_menu_select(current_map);
			SFX_PLAY(SFX_SELECT);
		} else if (rdc == 'r') {
			current_map = rand8() % maps_count;
			main_menu_select(current_map);
			SFX_PLAY(SFX_SELECT);
		}

		rdb = rda;
	}
}