@echo off

mkdir ..\res 2> nul
del ..\res\*.bin /q /s

echo.
echo Building resources
echo ======================

echo.
echo Loading screen
echo --------------------------------------------------
..\utils\png2scr.exe ..\gfx\loading.png ..\res\loading.bin > nul

echo.
echo Title screen
echo --------------------------------------------------
..\utils\png2scr.exe ..\gfx\title.png ..\gfx\title.scr
..\utils\apack.exe ..\gfx\title.scr ..\res\title_c.bin
del ..\gfx\*.scr

echo.
echo Map assets
echo --------------------------------------------------
..\utils\rle53map_sp.exe in=..\map\maps.map out=..\res\ size=4,4 nodecos noindex scrsize=15,10

echo.
echo Sprites
echo --------------------------------------------------
cd ..\utils
..\utils\png2sprite\png2sprite.exe ..\gfx\sprites.png binary -i sprites
move ..\utils\sprites.bin ..\res\sprites.bin

echo.
echo Creating main pattern set and level-based tilemaps
echo --------------------------------------------------
cd ..\gfx
..\utils\mkts_om.exe platform=zx mode=scripted in=tileset.spt
cd ..\src

echo.
echo Assembling player
echo --------------------------------------------------
cd ..\mus
..\utils\pasmo.exe WYZproPlay47aZX.ASM ..\res\wyz_player.bin
..\utils\apack.exe ..\res\wyz_player.bin ..\res\wyz_player_c.bin >nul
del ..\res\wyz_player.bin
cd ..\src