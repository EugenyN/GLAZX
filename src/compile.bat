@echo off

SET pack=1

echo ### COMPILING ###

rem for z88dk < 1.99c (SCCZ80)
rem zcc +zx -vn main.c -o game.bin -zorg=24000 -lsp1 -lndos -lmalloc -lim2

rem for z88dk >= 1.99c and later (SCCZ80)
zcc +zx -vn main.c -o game.bin -zorg=24000 -lsp1 -lndos -DZ88DK199C

echo ### MAKING TAPS ###

if %pack%==1 (
	..\utils\apack.exe game.bin game_c.bin
	..\utils\apack.exe ..\res\loading.bin loading_c.bin
	..\utils\imanol.exe in=loader\loader-template-appack.asm out=loader.asm mainbincompstart=?65000-game_c.bin mainbincomplength=?game_c.bin loadingcompstart=?65000-loading_c.bin loadingcomplength=?loading_c.bin
	..\utils\pasmo.exe loader.asm loader.bin

	..\utils\GenTape.exe glazx.tap ^
		basic 'GLAZX' 10 loader.bin ^
		data             loading_c.bin ^
		data             game_c.bin
) else (
	copy ..\res\loading.bin loading.bin
	..\utils\imanol.exe in=loader\loader-template.asm out=loader.asm binsize=?game.bin
	..\utils\pasmo.exe loader.asm loader.bin

	..\utils\GenTape.exe glazx.tap ^
		basic 'GLAZX' 10 loader.bin ^
		data             loading.bin ^
		data             game.bin
)

echo ### CLEANING ###

del loading*.bin
del loader.asm
del loader.bin
del game*.bin
del zcc_opt.def

echo ### DONE ###