// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

#define BULLET_FIRST_FRAME_OFFSET 4

void glaz_hit(unsigned char p);
void hud_update(unsigned char p);

void bullets_init() 
{
	it = 0;
	while (it < MAX_BULLETS) {
		spr_on[SPR_BULLET_BASE + it] = 0;
		bullet_speed[it] = 1;
		it ++;
	}
}

void bullet_fire(unsigned char p) 
{	
	if (p_state[p] == STATE_EXPLODES)
		return;

	p_state[p] = STATE_ATTACK;
	p_state_frame_counter[p] = ATTACK_ANIMATION_TIME;

	if (p_ammo[p] == 0 || p_ghost_mode[p] == GHOST_MODE) {
		SFX_PLAY(SFX_NO_AMMO_FIRE);
		return;
	}

	// we are looking for a free bullet
	for (jt = 0; jt < MAX_BULLETS_PER_GLAZ; jt++) 
	{
		spr_idx = SPR_BULLET_BASE + jt + (p << 1); // p << MAX_BULLETS_PER_GLAZ - 1

		if (spr_on[spr_idx] == 0) 
		{
			rdx = spr_x[SPR_PLAYER_BASE + p];
			rdy = spr_y[SPR_PLAYER_BASE + p];

			switch (p_facing[p]) 
			{
				case FACING_LEFT:
					if (rdx != 0)
						rdx -= BULLET_FIRST_FRAME_OFFSET;
					spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE];
					break;
				case FACING_RIGHT:
					if (rdx != PX(MAP_WIDTH))
						rdx += BULLET_FIRST_FRAME_OFFSET;
					spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE];
					break;
				case FACING_DOWN:
					if (rdy != PX(MAP_HEIGHT))
						rdy += BULLET_FIRST_FRAME_OFFSET;
					spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE + 1];
					break;
				case FACING_UP:
					if (rdy != 16)
						rdy -= BULLET_FIRST_FRAME_OFFSET;
					spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE + 1];
					break;
			}

			spr_on[spr_idx] = 1;
			spr_x[spr_idx] = rdx;
			spr_y[spr_idx] = rdy;

			bullet_dir[jt + (p << 1)] = p_facing[p];
			bullet_speed[jt + (p << 1)] = 1;

			p_ammo[p]--;
			if (p_rocket_sight_counter[p] != 0) {
				p_rocket_sight_counter[p]--;
				bullet_speed[jt + (p << 1)] = 0xff;
			}

			hud_update(p);

			SFX_PLAY(SFX_FIRE);
			break;
		}
	}
}

void bullets_turn(unsigned char p) 
{
	spr_idx = SPR_BULLET_BASE + 0 + (p << 1); // get first bullet index

	if (bullet_dir[0 + (p << 1)] == p_facing[p])
	{
		// fast ray
		bullet_speed[0 + (p << 1)] = 2;
	}
	else 
	{
		// curved ray
		bullet_dir[0 + (p << 1)] = p_facing[p];
		if (p_facing[p] == FACING_LEFT || p_facing[p] == FACING_RIGHT)
			spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE];
		else
			spr_frame[spr_idx] = spr_frames[BULLET_FRAME_BASE + 1];
	}
}

unsigned char collide_with_player()
{
	for (jt = 0; jt < MAX_GLAZES; jt++) {
		if (p_state[jt] == STATE_EXPLODES || p_ghost_mode[jt])
			continue;
		if (p_state_frame_counter[jt] == ATTACK_ANIMATION_TIME - 1)
			continue;
		if (_x == TL(spr_x[SPR_PLAYER_BASE + jt]) &&
		 	_y == TL(spr_y[SPR_PLAYER_BASE + jt])) {
			glaz_hit(jt);
			return 1;
		}
	}
	return 0;
}

unsigned char bullets_move_one_step()
{
	// first pass
	if (collide_with_player())
		return 1;

	switch (bullet_dir[it]) 
	{
		case FACING_LEFT:
			if (_x > bullet_speed[it] - 1)
				_x -= bullet_speed[it];
			else
				return 1;
			break;
		case FACING_RIGHT:
			if (_x < MAP_WIDTH - 1)
				_x += bullet_speed[it];
			else
				return 1;
			break;
		case FACING_DOWN:
			if (_y < MAP_HEIGHT - 1)
				_y += bullet_speed[it];
			else
				return 1;
			break;
		case FACING_UP:
			if (_y > bullet_speed[it] - 1)
				_y -= bullet_speed[it];
			else
				return 1;
			break;
	}

	// tiles collision
	if (get_attr() > 7)
		return 1;

	// bonus collision
	_t = get_tile();
	if (BONUS_TILE(_t)) {
		restore_map_tile();
		SFX_PLAY(SFX_BONUS_HIT);
		return 1;
	}

	// second pass
	if (collide_with_player())
		return 1;

	return 0;
}

void bullets_move() 
{
	for (it = 0; it < MAX_BULLETS; it++) 
	{
		spr_idx = SPR_BULLET_BASE + it;

		if (spr_on[spr_idx]) 
		{
			// "+ BULLET_FIRST_FRAME_OFFSET" - rounding correction
			_x = TL(spr_x[spr_idx] + BULLET_FIRST_FRAME_OFFSET);
			_y = TL(spr_y[spr_idx] + BULLET_FIRST_FRAME_OFFSET);

			// rocket sight
			if (bullet_speed[it] == 0xff)
			{
				bullet_speed[it] = 1;
				while (bullets_move_one_step() == 0) { }
				spr_on[spr_idx] = 0;
			}
			else
			{
				if (bullets_move_one_step() == 0)
				{
					spr_x[spr_idx] = PX(_x);
					spr_y[spr_idx] = PX(_y);
				}
				else
					spr_on[spr_idx] = 0;
			}
		}
	}
}