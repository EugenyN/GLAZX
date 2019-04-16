![GLAZX logo](/web/logo.png)

# GLAZX

a small, open-source game for **ZX Spectrum 48/128** in classic arcade style.

The game is developed on С and Z80 assembler with using **Z88DK** dev kit and **Sprite Pack / SP1** library. The GLAZX is designed as local multiplayer game and the current version only supports multiplayer for two players. The game was created as an interesting challenge and dev experience for the computer on which I played my first game sometime in 1992.

![scr1](/web/scr1.png) ![scr2](/web/scr2.png)
![scr3](/web/scr3.png) ![scr4](/web/scr4.png)
![scr4](/web/scr5.png) ![scr5](/web/scr6.png)

## How to play

The **GLAZX** is a very simple action game in which the reaction speed is more important than solving puzzles or strategic planning.

The glazes are placed on a **grid-like map** with passable cells and obstacles, and can be moved to the right one cell. The glaz emits **deadly rays**. The wall prevents your moving and stops rays. Randomly **bonus boxes** will appear on the map with energy, additional rays and other bonus abilities. Immediately after damaging the glaz can not shoot for a while and can not be damaged again. It's called **"ghost time"**. The winner is the glaz that **destroyed all** opponents.

There are **several modifications** of the game which can be selected in the options. "NORMAL" and "PRO" slightly different lives and rays count, and other parameters. In the "NO GHOST" there are a lot of lives and there is no ghost time. In "ONE SHOT" you have only one shot, the bonus also gives only one extra shot, but the bonuses appear frequently.

The game includes 12 maps (+ 4 secret map), 4 types of bonuses, 4 game modes, 2 special attack movements. Zx 128 version contains AY chip title music and advanced sound effects.

Bonus boxes:

* **Live** - adds 1 life.
* **Ammo** - refills ammo. Adds 5 rays.
* **Super ghost time** - adds 10 sec. The glaz can shoot during the super ghost time.
* **Rocket Sight** - adds 1 shot of instant death ray, it is impossible to avoid this rocket ray.

Special moves:

* **Curved ray** - the glaz can turn the ray after the shot. Press and hold **Fire** + move in the **any direction** except forward.
* **Fast ray** - the glaz can emit a super fast ray. Wherein the hit probability is 50%. Also with a 50% probability the ray can go through the wall. Press and hold **Fire** + **forward direction** move. (in the same direction the shot was made)

Keyboard control:

* **GLAZ 1** - W, A, S, D, F
* **GLAZ 2** - I, J, K, L, O
* **Pause game** - N, **Exit game** - M

In the options menu you can choose the alternative control including **Kempston** and **Sinclair** joystick.

## Download

Ready for play .tap files:
* [GLAZX v1.0](https://github.com/EugenyN/GLAZX/releases/download/v1.0/glazx48.tap) for Spectrum 48
* [GLAZX v1.0 128](https://github.com/EugenyN/glazx/releases/download/v1.0/glazx128.tap) for Spectrum 128

## How to load

* Spectrum 128/+2/+3: to load the 128 Version select "Loader" from the main menu and press ENTER, then PLAY on tape.
* Spectrum 48/+: to load 48 Version type LOAD "" and press ENTER, then PLAY on tape.
* If you don’t have the hardware, you can still play it in an emulator. E.g. you can use [Fuse](http://fuse-emulator.sourceforge.net/) for Windows and Linux.
* You can also [play it online](http://torinak.com/qaop) thanks to Qaop/JS.
 
## How to build

* Z88DK installation:
	* Download the latest precompiled version of z88dk from [site](http://nightly.z88dk.org/) and unpack the archive for example in `c:\z88dk`. The game can also be compiled on early versions of z88dk. See comments in `dev\compile.bat`
	* Set the environment variables using `libs\setenv-z88dk.bat`. We assume sdk is installed in the `c:\z88dk`. More information about the z88dk installation and manual compilation can be found [here](https://github.com/z88dk/z88dk/wiki/installation).
* SP1 library compilation:
	* Enter `make sp1-spectrum` from the `{z88dk}/libsrc` directory to build the library. Header file `sp1.h` will be created in the `{z88dk}/include/sprites` directory.
	* Then enter `make install` to copy `sp1.lib` from `{z88dk}/libsrc` to `z88dk/lib/clibs`
* Or you can download z88dk with precompiled and installed `sp1.lib` [here](/libs/z88dk-win32-20190408.zip).
* GLAZX compilation:
	* To build resources, compile and create a .tap file use `dev\buildbins.bat` and `dev\compile.bat`.

The machine RAM and resources of the Z80 processor are very limited, so we use various optimization tricks: there are no dynamic data types, it's used minimum variables on stack, global variables are reused, data compression algorithms (rle, appack) are used, bit shifts instead of multiplication and division. The code and resources should be directly mapped in RAM to specific addresses, memory paging are used for 128 ver. Debugging is only in assembler.

## To do

* AI and pathfinding.
* Support for 3 and 4 players / cpu opponents.
* More maps, more modes, more optimizations.

## Credits

* Idea, code and graphics: Eugeny Novikov
* Additional art: Lorc, Delapouite, font: "I can remember" NES game, and tiles from some retro games.
* AY title music "Red Alert! (conversion from Metal Gear game)" by WYZ
* Beeper title music "Grandstand Theme" by Chris Cowley, some SFX by Shiru

## Third party code and tools:

* [Z88DK](https://www.z88dk.org) and Sprite Pack / SP1 library by Alvin Albrecht
* [The Mojon Twins](https://github.com/mojontwins) tools, ideas and code samples.
* [png2sprite](https://github.com/reidrac/png2sprite) tool by Juan J. Martinez
* PSG Player by WYZ, Phaser1 player by Shiru, Beepola by Chris Cowley
* aPPack decompressor by dwedit, adapted by Utopian and optimized by Metalbrain
* [Mappy](https://tilemap.co.uk/mappy.php) used for maps creation.

## License

Copyright (c) 2019 Eugeny Novikov. Code under MIT license. Art under CC-BY-SA 3.0.
