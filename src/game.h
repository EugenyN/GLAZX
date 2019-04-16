// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

#define MESSAGE_COLOUR (BRIGHT | INK_WHITE | PAPER_BLUE)

void game_init()
{
	games_played = 0;
	game_mode = MOD_NORMAL;
	maps_count = MAP_COUNT - 4;
	current_map = 0;
}

void hud_update(unsigned char p)
{
	rda = STAT_X + p * 12;
	print_number2(rda + 1, STAT_Y, INK_WHITE, p_life[p]);
	print_number2(rda + 5, STAT_Y, INK_WHITE, p_ammo[p]);
	print_str(rda + 4, STAT_Y, (INK_GREEN + p) | (p_rocket_sight_counter[p] != 0 ? FLASH : 0), BULLET_SYMBOL);
}

void hud_init()
{
	for (it = 0; it < MAX_GLAZES; it++) {
		print_str(STAT_X + 12 * it, STAT_Y, INK_GREEN + it, GLAZ_SYMBOL);
		hud_update(it);
	}

	print_str(TIMER_X, TIMER_Y, 71, "TIME:");
}

void hud_update_for_timer()
{
	if (timer.count == 1)
		print_number2(TIMER_X + 6, TIMER_Y, INK_WHITE, timer.t);
}

void game_mode_init()
{
	switch (game_mode)
	{
		case MOD_NORMAL:
			gm_config.start_lives = 5;
			gm_config.start_ammo = 20;
			gm_config.bonus_spawn_delay = 8;
			gm_config.ghost_time = 50;
			gm_config.ammo_refill = 10;
			break;
		case MOD_PRO:
			gm_config.start_lives = 3;
			gm_config.start_ammo = 10;
			gm_config.bonus_spawn_delay = 8;
			gm_config.ghost_time = 40;
			gm_config.ammo_refill = 5;
			break;
		case MOD_NO_GHOST:
			gm_config.start_lives = 20;
			gm_config.start_ammo = 40;
			gm_config.bonus_spawn_delay = 5;
			gm_config.ghost_time = 0;
			gm_config.ammo_refill = 10;
			break;
		case MOD_ONE_SHOT:
			gm_config.start_lives = 5;
			gm_config.start_ammo = 1;
			gm_config.bonus_spawn_delay = 2;
			gm_config.ghost_time = 10;
			gm_config.ammo_refill = 1;
			break;
		default:
			break;
	}
}

void level_init()
{
	timer_init();
	game_mode_init();
	bullets_init();
	glaz_init();

	bonus_delay = gm_config.bonus_spawn_delay + rand8() % gm_config.bonus_spawn_delay;
	game_end_result = GAME_END_NONE;

	 if (current_map == (maps_count - 1))
	 	generate_random_map(10);
	 if (current_map == (maps_count - 2))
	 	generate_random_map(20);

	draw_map();
	hud_init();

	glaz_spawn();
}

void title_screen_show()
{
	screen_unpack(title_scr); 
	MUSIC_PLAY(MUSIC_TITLE);

	while (1) {
		it = in_Inkey() - '0';
		if (it >= 1 && it <= 3) 
			break;

		// we have problems with lots of emulators when reading Kempston
		// if (in_JoyKempston() & in_FIRE)
		// {
		// 	it = 2;
		// 	break;
		// }	
	}

	switch (it)
	{
		case 1:
			controls_setup(0, CTRL_KEYBOARD1);
			break;
		case 2:
			controls_setup(0, CTRL_KEMPSTON);
			break;
		case 3:
			controls_setup(0, CTRL_SINCLAIR1);
			break;
		default:
			break;
	}

	// glaz 2 - always keyboard. use "set controls" menu for advanced setup
	controls_setup(1, CTRL_KEYBOARD2);

	SFX_PLAY(SFX_SELECT);
	in_WaitForNoKey();
}

void print_message(unsigned char *s) 
{
	print_str(10, 11, MESSAGE_COLOUR, "            ");
	print_str(10, 12, MESSAGE_COLOUR, s);
	print_str(10, 13, MESSAGE_COLOUR, "            ");
}

void pause_message() 
{
	print_message("   PAUSED   ");
	sprites_hide_all();
	SCREEN_UPDATE;
	SFX_PLAY(SFX_PAUSE);
	in_WaitForNoKey();
	while (in_KeyPressed(PAUSE_KEY) == 0);
	in_WaitForNoKey();
}

void exit_message() 
{
	print_message("   EXIT !   ");
	sprites_hide_all();
	SCREEN_UPDATE;
	SFX_PLAY(SFX_PAUSE);
	in_WaitForNoKey();
	wait_button(100);
}

void win_message() 
{
	print_message(" GLAZ # WIN ");
	print_number1(16, 12, MESSAGE_COLOUR, get_more_healthy_glaz() + 1);
	sprites_hide_all();
	SCREEN_UPDATE;
	SFX_PLAY(SFX_ROUND_FINISHED);
	wait_button(150);
}

void time_up_message()
{
	print_message("  TIME UP!  ");
	sprites_hide_all();
	SCREEN_UPDATE;
	SFX_PLAY(SFX_ROUND_FINISHED);
#ifdef MODE_128K
	wait(50);
#endif
	wait_button(50);
	wait_no_button();

	rda = get_more_healthy_glaz();
	if (rda != 0xff)
	{
		print_message(" GLAZ # WIN ");
		print_number1(16, 12, MESSAGE_COLOUR, rda + 1);
	}
	else 
	{
		print_message(" NOBODY WON ");
	}
	SCREEN_UPDATE;
	wait_button(100);
}

void level_end() 
{
	switch (game_end_result)
	{
		case GAME_END_EXIT:
		{
			exit_message();
			break;
		}
		case GAME_END_GLAZ_WIN:
		{
			wait(50);
			win_message();
			games_played++;
			break;
		}
		case GAME_END_TIME_UP:
		{
			time_up_message();
			games_played++;
			break;
		}
		default:
			break;
	}

	if (games_played >= GAMES_PLAYED_FOR_SECRET)
		maps_count =  MAP_COUNT;
}

void pause_abort_keys_process()
{
	if (in_KeyPressed(PAUSE_KEY)) 
	{
		pause_message();
		draw_map();
	}

	if (in_KeyPressed(EXIT_KEY)) 
	{
		do_game = 0;
		game_end_result = GAME_END_EXIT;
	}
}

void bonuses_process()
{
	if (timer.count == 0)
		bonus_delay--;

	if (bonus_delay == 0) 
	{
		if (game_mode == MOD_ONE_SHOT && (rand8() & 3))
			_t = AMMO_TILE;
		else 
			_t = AMMO_TILE + rand8() % 4;
		_x = rand8() % MAP_WIDTH; 
		_y = rand8() % MAP_HEIGHT;
		if (get_attr () == 0) {
			set_tile();
			SFX_PLAY(SFX_BONUS_SPAWN);
		}

		bonus_delay = gm_config.bonus_spawn_delay + rand8() % gm_config.bonus_spawn_delay;
	}
}

void game_loop()
{
	do_game = 1;

	while (do_game) 
	{
		timer_process();
		hud_update_for_timer();
		bonuses_process();
		bullets_move();
		glazes_process();
		glazes_animate();
		pause_abort_keys_process();
		sprites_update();
		SCREEN_UPDATE;

		if (timer.zero) {
			do_game = 0;
			game_end_result = GAME_END_TIME_UP;
		}

		if (frames == 0) {
			#asm
				halt
			#endasm
		}

		if (frames < 2) {
			#asm
				halt
			#endasm
		}

		frames = 0;
	}
}

void main() 
{
	system_init();
	sprite_frames_init();
	sprites_init();
	SE_INIT();
	game_init();
	screen_fade_out();

	while (1) {
		title_screen_show();
		main_menu();
		MUSIC_STOP();
		level_init();
		game_loop();
		level_end();
		screen_fade_out();
	}
}