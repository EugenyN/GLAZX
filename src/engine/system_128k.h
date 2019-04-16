// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

void set_ram_bank() 
{
	#asm
		.SetRAMBank
			ld	a, ($5b5c)
			and $f8
			or	b
			ld	bc, $7ffd
			ld	($5b5c), a
			out (C), a
	#endasm
}