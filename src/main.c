// Copyright (c) 2019 Eugeny Novikov. Code under MIT license.

#define MODE_128K

#pragma output STACKPTR=23999 //53248 for sp1 ? dont use hex constant.

#include <sprites/sp1.h>
#include <input.h>
#include <im2.h>
#include <string.h>
#include <malloc.h>

#include "definitions.h"
#include "globals.h"
#include "resources.h"

#include "engine/aplib.h"
#include "engine/splib.h"
#ifdef MODE_128K
	#include "engine/system_128k.h"
#endif
#ifdef SE_WYZ
	#include "sound/wyzplayer.h"
#endif
#ifdef SE_BEEPER
	#include "sound/beeper_sfx.h" // should be in fast memory ! 0x8000-0xFFFF ?
	#include "sound/beeper_music.h"
#endif
#include "engine/random.h"
#include "engine/engine.h"
#include "engine/map.h"

#include "bullets.h"
#include "menu.h"
#include "glaz.h"
#include "game.h"
