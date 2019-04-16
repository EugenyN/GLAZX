// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

extern const unsigned char tileset[0];
extern const unsigned char tileset_map[0];
extern const unsigned char map_data[0];
extern const unsigned char sprites_data[0];
extern const unsigned char title_scr[0];
#ifdef MODE_128K
extern const unsigned char wyz_player[0];
#endif

#asm
	;; Patterns and tilemaps for the metatileset
	._tileset
		; main font. 64 chars (512 bytes) long.
		; BINARY "../res/font.bin"
		; space for the remaining 75 characters (600 bytes).
		; defs 600
		BINARY "../res/ts.bin" ; 1112 bytes

	._tileset_map
		; 24 metatiles (192 bytes).
		; defs 192
		BINARY "../res/ts.tilemaps.bin"

	;; Map data
	._map_data
		BINARY "../res/map.bin"

	;; Sprite frames
	._sprites_data
		BINARY "../res/sprites.bin"
		
	;; Title screen
	._title_scr
		BINARY "../res/title_c.bin"

#ifdef MODE_128K
	;; Music and player
	._wyz_player
		BINARY "../res/wyz_player_c.bin"
#endif
#endasm

/* file end. this fix zcc compiler issue (1.99c and later, SCCZ80 ?) */