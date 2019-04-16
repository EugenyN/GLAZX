; by na_th_an - Thanks to Antonio Villena for his tutorials and utilities.

	org $5ccb
	ld sp, 23999 ;$ffff
	di
	db	$de, $c0, $37, $0e, $8f, $39, $96 ;OVER USR 7 ($5ccb)
	
; screen 0
	ld	hl, $5800
	ld	de, $5801
	ld	bc, 768
	ld	(hl), l
	ldir

; load screen
	scf
	ld	a, $ff
	ld	ix, $4000
	ld	de, $1b00
	call $0556

; load binary
	scf
	ld	a, $ff
	ld	ix, 24000
	ld	de, %%%binsize%%%
	call $0556

; run game!
	jp 24000
	