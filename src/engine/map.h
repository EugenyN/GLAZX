// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.
// Copyleft 2017, 2018 by The Mojon Twins

#define MAP_FORMAT_RLE53
#define MAP_CHARSWIDTH		MAP_WIDTH * 2

// Screen tiles

unsigned char get_attr() 
{
	return scr_attr[(_y << 4) - _y + _x];
}

unsigned char get_tile() 
{
	return scr_buff[(_y << 4) - _y + _x];
}

void set_tile() 
{
	rda = (_y << 4) - _y + _x;
	scr_buff[rda] = _t; 
	scr_attr[rda] = tile_attr[_t];
	_x = SCR_X + (_x << 1);
	_y = SCR_Y + (_y << 1);
	print_tile();
}

void draw_tiles_debug()
{
	for (it = 0; it < MAP_HEIGHT; it++) {
		for (jt = 0; jt < MAP_WIDTH; jt++) {
			_x = jt;
			_y = it;
			print_number2(1 + (jt << 1), 2 + (it << 1), INK_WHITE, get_tile());
		}
	}
}

// Map

unsigned char *map_base_address() 
{
	// random map array
	if (current_map > maps_count - 3)
		return random_map_data;

#ifdef MAP_FORMAT_RLE53
	rdc = current_map << 1;
	rda = *(map_data + rdc); 
	rdb = *(map_data + rdc + 1);
	return (unsigned char *) (map_data + ((rdb << 8) | rda));
#else
	return (unsigned char *) (map_data + current_map * 150);
#endif
}

void draw_map_advance() 
{
	_t = rdt;
	scr_buff[it] = _t; 
	scr_attr[it] = tile_attr[_t];

	print_tile();

	_x += 2; 
	if (_x == MAP_CHARSWIDTH + SCR_X) { 
		_x = SCR_X; 
		_y += 2;
	}

	it++;
}

void draw_map()
{
	gp_map = map_base_address();
	it = 0; _x = SCR_X; _y = SCR_Y;

	#ifdef MAP_FORMAT_RLE53	
		while (it < SCR_BUFFER_SIZE) {
			rdt = *gp_map ++;
			rdct = 1 + (rdt >> 5);
			rdt &= 0x1f;
			while (rdct --) 
				draw_map_advance();
		}
	#else
		while (it < SCR_BUFFER_SIZE) {
			rdt = *gp_map ++;
			draw_map_advance();
		}
	#endif
}

#if defined(MAP_FORMAT_RLE53)
	// not implemented
#else
void set_map_tile()
{
	*((unsigned char *) (map_base_address () + rdx + (rdy + (rdy << 1) + (rdy << 2) + (rdy << 3)))) = rdt;
}
#endif

void get_map_tile()
{
	gp_map = map_base_address();
	rda = (_y << 4) - _y + _x;

#if defined(MAP_FORMAT_RLE53)
	rdi = 0; rdx = SCR_X;
	while (rdi < SCR_BUFFER_SIZE) 
	{
		rdt = *gp_map ++;

		// for MAP_FORMAT_RLE44
		// rdct = 1 + (rdt >> 4);
		// rdt &= 0x0f;
		rdct = 1 + (rdt >> 5);
		rdt &= 0x1f;

		while (rdct--) 
		{
			if (rdi == rda)
			{
				_t = rdt;
				return;
			}

			rdx += 2; 
			if (rdx == MAP_CHARSWIDTH + SCR_X)
				rdx = SCR_X; 
			
			rdi++;
		}
	}
#else
	// not implemented
#endif
}

void restore_map_tile() 
{
	get_map_tile();
	set_tile();
}

void generate_random_map(unsigned char pass_chance)
{
	rdi = (rand8() % 4) << 2; // level tileset
	it = 0;
	while (it < SCR_BUFFER_SIZE) {
		rdt = (rand8() & pass_chance) ? 0 : 2; // wall or passable
		rdt += 1 + rdi + ((rand8() & 3) ? 1 : 0); // tile variant
		random_map_data[it] = rdt;
		it++;
	}
}