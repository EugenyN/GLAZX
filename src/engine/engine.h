// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

long heap;   // the four-byte heap pointer required by the malloc library to keep track of malloc's heap

void *u_malloc(unsigned int size) {
	return malloc(size);
}

void u_free(void *addr) {
	free(addr);
}

// interrupts

void isr_callback()
{
	frames++;
	SE_ISR();
}

void interrupts_init()
{
#asm
	di
#endasm
	im2_Init(0xd000); // place z80 in im2 mode with interrupt vector table located at 0xd000
	memset(0xd000, 0xd1, 257); // create a 257-byte im2 vector table containing 0xd1 bytes
	im2_CreateGenericISR(1, 0xd1d1); // create a Generic ISR at address 0xd1d1 with space for hooks
	im2_RegHooKFirst(255, isr_callback);
#asm
	ei
#endasm
}

void system_init() 
{
#if defined(MODE_128K) && defined(SE_WYZ)
	unpack_to_bank(1, wyz_player, (unsigned char *) (BASE_SOUND)); // unpack sound to RAM1
#endif
	interrupts_init();

	// reserve memory blocks for sprites
	heap = 0L;  // malloc's heap is empty
	sbrk(MALLOC_ADDR - SW_SPRITES_MEMORY, SW_SPRITES_MEMORY); // add memory to malloc's heap

	zx_border(INK_BLACK);

	splib_init();
	srand_use_system_clock();
}

void sprite_frames_init() 
{
	rda = 0; 

	// 16x16 frames
	gp_gen = sprites_data; 
	it = MAX_FRAMES_16x16; while (it--) {
		spr_frames[rda ++] = gp_gen;
		gp_gen += 96;
	}

	//TODO: add support for 8x8, 24x24 etc...
}

void screen_fade_out() 
{
	#asm
		ld b, 7
	.fade_out_extern
		push bc

		ld   e, 3               ; 3 thirds
		ld   hl, 22528          ; here the attributes begin
	#endasm
#ifdef MODE_128K
	#asm
		halt                    ; we wait for delay.
	#endasm
#endif
	#asm
	.fade_out_loop
		ld   a, (hl )           ; we bring the current attribute

		ld   d, a               ; take attribute
		and  7                  ; isolate the ink
		jr   z, ink_done        ; jumps if Z is set
		dec  a                  ; decrease ink
	.ink_done
		ld   b, a               ; in b we now have the ink already processed.

		ld   a, d               ; take attribute
		and  56                 ; Isolate the paper, without changing its position in the byte
		jr   z, paper_done      ; jumps if Z is set
		sub  8                  ; we decrease paper by subtracting 8
	.paper_done
		ld   c, a               ; in c we have now the paper already processed.
		ld   a, d
		and  192                ; we are left with bits 6 and 7 (BRIGHT and FLASH)
		or   c                  ; add paper
		or   b                  ; and ink, with which recomposed the attribute
		ld   (hl),a             ; write,
		inc  l                  ; and increase the pointer.
		jr   nz, fade_out_loop  ; continue until finishing the third (when L is equal to 0)
		inc  h                  ; next third
		dec  e
		jr   nz, fade_out_loop ; repeat 3 times
		pop bc
		djnz fade_out_extern
	#endasm
}

void screen_unpack(unsigned char * address)
{
	SCREEN_UPDATE;
	clear_game_area();
	unpack(address, (unsigned char *) (0x4000));
}

// workaround for fastcall function / fn pointer incompatibility. for >= 1.99c
#ifdef Z88DK199C
unsigned int in_JoyKeyboard_fix(struct in_UDK *u)
{
	return in_JoyKeyboard(u);
}
#endif

void controls_setup(unsigned int p, unsigned int ctrl)
{
	switch (ctrl) {
		case CTRL_KEYBOARD1:
#ifdef Z88DK199C		
			joyfunc[p] = in_JoyKeyboard_fix;
#else
			joyfunc[p] = in_JoyKeyboard;
#endif
			keys[p].up = in_LookupKey('w');
			keys[p].down = in_LookupKey('s');
			keys[p].left = in_LookupKey('a');
			keys[p].right = in_LookupKey('d');
			keys[p].fire = in_LookupKey('f');
			break;
		case CTRL_KEYBOARD2:
#ifdef Z88DK199C		
			joyfunc[p] = in_JoyKeyboard_fix;
#else
			joyfunc[p] = in_JoyKeyboard;
#endif
			keys[p].up = in_LookupKey('i');
			keys[p].down = in_LookupKey('k');
			keys[p].left = in_LookupKey('j');
			keys[p].right = in_LookupKey('l');
			keys[p].fire = in_LookupKey('o');
			break;
		case CTRL_KEMPSTON:
			joyfunc[p] = in_JoyKempston;
			break;
		case CTRL_SINCLAIR1:
			joyfunc[p] = in_JoySinclair1;
			break;
		case CTRL_SINCLAIR2:
			joyfunc[p] = in_JoySinclair2;
			break;
	}
}

unsigned char is_button_pressed() 
{
	return (in_Inkey() || INPUT_FIRE(INPUT_READ(0)));
}

void wait_no_button()
{
	// in_WaitForNoKey() does not take into account the joystick
	while (is_button_pressed());
}

void wait_button(unsigned char delay)
{
	frames = 0;
	while (frames < delay)
	{
		if (is_button_pressed()) 
			break;
	}
}

void wait(unsigned char delay) 
{
	frames = 0;
	while (frames < delay);
}

void timer_init()
{
	timer.count = 0;
	timer.zero = 0;
	timer.frames = TIMER_LAPSE;
	timer.t = TIMER_INITIAL;
	timer.on = 1;
}

void timer_process()
{
	if (timer.on) {
		timer.count ++;
		if (timer.count == timer.frames) {
			timer.count = 0;
			if (timer.t) {
				timer.t--;
			} else {
				timer.zero = 1;
			}
		}
	}
}

void play_step_sound() 
{
	#asm
		ld a, 16
		out (254), a
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		xor 16
		out (254), a
	#endasm
}
