// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

unsigned char get_more_healthy_glaz()
{
	rda = 0xff;
	rdb = 0xff;

	for (it = 0; it < MAX_GLAZES; it++) {
		if (p_life[it] < rdb)
			rdb = p_life[it];
	}

	for (it = 0; it < MAX_GLAZES; it++) {
		if (p_life[it] > rdb) {
			rdb = p_life[it];
			rda = it;
		}
	}
	return rda;
}

unsigned char get_winner_glaz()
{
	rda = 0xff;
	for (it = 0; it < MAX_GLAZES; it++) {
		if (p_life[it] != 0) {
			if (rda != 0xff)
				return 0xff;
			rda = it;
		}
	}
	return rda;
}

void glaz_init() 
{
	for (it = 0; it < MAX_GLAZES; it++) 
	{
		spr_idx = SPR_PLAYER_BASE + it;

		p_facing[it] = FACING_DOWN;

		p_life[it] = gm_config.start_lives;
		p_ammo[it] = gm_config.start_ammo;
		p_rocket_sight_counter[it] = 0;
		p_state[it] = STATE_IDLE;
		p_ghost_mode[it] = 0;
		p_state_frame_counter[it] = 0;

		sprite_set_color(spr_idx, INK_GREEN + spr_idx);
	}

	p_idle_frame = 0;
	p_idle_frame_counter = IDLE_ANIMATION_TIME;
}

void glaz_spawn() 
{
	for (it = 0; it < MAX_GLAZES; it++) 
	{
		spr_idx = SPR_PLAYER_BASE + it;

		// try 10 times
		for (jt = 0; jt < 10; jt++) 
		{
			_x = rand8() % MAP_WIDTH; 
			_y = rand8() % MAP_HEIGHT;
			if (get_attr() == 0)
				break;
		}

		spr_x[spr_idx] = PX(_x);
		spr_y[spr_idx] = PX(_y);
		spr_on[spr_idx] = 1;
	}
}

void glazes_animate()
{
	if (p_idle_frame_counter == 0) {
		p_idle_frame_counter = IDLE_ANIMATION_TIME;
		p_idle_frame = !p_idle_frame;
	}
	p_idle_frame_counter--;

	for (it = 0; it < MAX_GLAZES; it++) 
	{
		spr_idx = SPR_PLAYER_BASE + it;

		switch (p_state[it])
		{
			case STATE_IDLE:
				spr_frame[spr_idx] = spr_frames[(p_facing[it] << 1) + p_idle_frame];
				break;
			case STATE_ATTACK:
				spr_frame[spr_idx] = spr_frames[p_facing[it] + ATTACK_FRAME_BASE];
				if (p_state_frame_counter[it] == 0)
					p_state[it] = STATE_IDLE;
				break;
			case STATE_EXPLODES:
			{
				if (p_state_frame_counter[it] > 4) {
					spr_frame[spr_idx] = spr_frames[EXPLOSION_FRAME_BASE + 1];
				} else if (p_state_frame_counter[it] > 0) {
					spr_frame[spr_idx] = spr_frames[EXPLOSION_FRAME_BASE];
				} else {
					p_state[it] = STATE_IDLE;
					if (gm_config.ghost_time != 0) {
						p_ghost_mode[it] = GHOST_MODE;
						p_ghost_mode_counter[it] = gm_config.ghost_time;
					}

					if (game_end_result != GAME_END_NONE)
					{
						spr_on[spr_idx] = 0;
						do_game = 0;
					}
				}
				break;
			}
			default:
				break;
		}

		if (p_ghost_mode[it])
		{
			p_ghost_mode_counter[it]--;
			if (p_ghost_mode_counter[it] % 5 == 0)
				spr_on[spr_idx] = !spr_on[spr_idx];
			if (p_ghost_mode_counter[it] == 0) {
				spr_on[spr_idx] = 1;
				p_ghost_mode[it] = 0;
			}
		}


		if (p_state_frame_counter[it] > 0)
			p_state_frame_counter[it]--;
	}
}

void glaz_get_bonus(unsigned char p) 
{
	switch (_t)
	{
		case AMMO_TILE:
		{
			p_ammo[p] = MIN(p_ammo[p] + gm_config.ammo_refill, gm_config.start_ammo);
			break;
		}
		case LIVE_TILE:
		{
			p_life[p] = MIN(p_life[p] + 1, gm_config.start_lives);
			break;
		}
		case GHOST_TILE:
		{
			p_ghost_mode[p] = SUPER_GHOST_MODE;
			p_ghost_mode_counter[p] = SUPER_GHOST_MODE_TIME;
			break;
		}
		case ROCKET_TILE:
		{
			p_ammo[p] = MIN(p_ammo[p] + ROCKET_AMMO_REFILL, gm_config.start_ammo);
			p_rocket_sight_counter[p] += ROCKET_AMMO_REFILL;
			break;
		}
		default:
			break;
	}

	hud_update(p);
	restore_map_tile();
	SFX_PLAY(SFX_GET_BONUS);
}

void glaz_try_move(unsigned char p)
{
	spr_idx = SPR_PLAYER_BASE + p;
	_x = TL(spr_x[spr_idx]);
	_y = TL(spr_y[spr_idx]);

	switch (p_facing[p]) 
	{
		case FACING_LEFT:
			if (_x == 0)
				_x = MAP_WIDTH - 1;
			else
				_x--;
			break;
		case FACING_RIGHT:
			if (_x == MAP_WIDTH - 1)
				_x = 0;
			else
				_x++;
			break;
		case FACING_DOWN:
			if (_y == MAP_HEIGHT - 1)
				_y = 0;
			else
				_y++;
			break;
		case FACING_UP:
			if (_y == 0)
				_y = MAP_HEIGHT - 1;
			else
				_y--;
			break;
	}

	// obstacle
	if (get_attr() & OBSTACLE_TILE_ATTR)
		return;

	// collision with other player
	for (jt = 0; jt < MAX_GLAZES; jt++) {
		if (p != SPR_PLAYER_BASE + jt)
		{
			if (TL(spr_x[SPR_PLAYER_BASE + jt]) == _x && 
				TL(spr_y[SPR_PLAYER_BASE + jt]) == _y)
				return;
		}
	}

	spr_x[spr_idx] = PX(_x);
	spr_y[spr_idx] = PX(_y);

	// bonus
	_t = get_tile();
	 if (BONUS_TILE(_t)) 
	 	glaz_get_bonus(it);

	play_step_sound();
}

unsigned char glazes_process() 
{
	for (it = 0; it < MAX_GLAZES; it++)
	{
		pad_current[it] = INPUT_READ(it);

		if (pad_current[it] != pad_last[it])
		{
			rda = 0;
			if (INPUT_UP_PRESSED(pad_current[it], pad_last[it])) {
				p_facing[it] = FACING_UP; rda = 1;
			} else if (INPUT_DOWN_PRESSED(pad_current[it], pad_last[it])) {
				p_facing[it] = FACING_DOWN; rda = 1;
			} else if (INPUT_LEFT_PRESSED(pad_current[it], pad_last[it])) {
				p_facing[it] = FACING_LEFT; rda = 1;
			} else if (INPUT_RIGHT_PRESSED(pad_current[it], pad_last[it])) {
				p_facing[it] = FACING_RIGHT; rda = 1;
			}

			if (INPUT_FIRE_PRESSED(pad_current[it], pad_last[it]) )
				bullet_fire(it);

			if (rda == 1) {
				if (INPUT_FIRE(pad_current[it]))
					bullets_turn(it);
				else
					glaz_try_move(it);
			}

			pad_last[it] = pad_current[it];
		}
	}
}

void glaz_hit(unsigned char p) 
{
	p_life[p]--;

	hud_update(p);

	if (p_life[p] != 0)
	{
		// glaz damaged
		SFX_PLAY(SFX_GLAZ_HIT);

		p_state[p] = STATE_EXPLODES;
		p_state_frame_counter[p] = EXPL_ANIMATION_TIME;
	} 
	else 
	{
		// glaz killed
		SFX_PLAY(SFX_DEATH);

		p_state[p] = STATE_EXPLODES;
		p_state_frame_counter[p] = EXPL_ANIMATION_TIME;

		// round end ?
		if (get_winner_glaz() != 0xff)
			game_end_result = GAME_END_GLAZ_WIN;
	}
}