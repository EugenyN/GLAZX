// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.
// Code by Patrik Rak, adapted by na_th_an
// http://www.boriel.com/wiki/en/index.php/ZX_BASIC:RandomStream.bas

extern unsigned int seed1;
extern unsigned int seed2;
extern unsigned char randres;

#asm
	._seed1 defw 0
	._seed2 defw 0
	._randres defb 0
#endasm

unsigned char rand8()
{
	#asm
		.rnd 
			ld	hl,0xA280
			ld	de,0xC0DE
			ld	a,h			; t = x ^ (x << 1)
			add a,a
			xor h
			ld	h,l			; x = y
			ld	l,d			; y = z
			ld	d,e			; z = w
			ld	e,a
			rra				; t = t ^ (t >> 1)
			xor e
			ld	e,a
			ld	a,d			; w = w ^ ( w << 3 ) ^ t
			add a,a
			add a,a
			add a,a
			xor d
			xor e
			ld	e,a
			ld	(rnd+1),hl
			ld	(rnd+4),de
			ld	(_randres), a
	#endasm
	return randres;
}

void srand8() 
{
	#asm
			ld	hl, (_seed1)
			ld	(rnd+1),hl
			ld	hl, (_seed2)
			ld	(rnd+4),hl
	#endasm
}

void system_clock()
{
	#asm
		di
		ld de,(23674)
		ld d,0
		ld hl,(23672)
		ei
	#endasm
}

void srand_use_system_clock() 
{
	// updates the random generator seed from the FRAMES system variable.
	system_clock();

	#asm
		ld a,e
		ex de,hl
		ld hl,rnd+2
		xor (hl)
		and a
		jr nz,updateSeedNotZero
		inc a
		updateSeedNotZero:
		ld (hl),a
		
		ld hl,rnd+4
		ld a,e
		xor (hl)
		ld (hl),a
		inc hl
		ld a,d
		xor (hl)
		ld (hl),a
	#endasm
}
