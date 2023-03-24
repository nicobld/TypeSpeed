#pragma once
#include "game.h"

#define BUFSIZE 0x10000
#define NUM_OF_WORDS 988
#define WORD_LENGTH 16

extern char buf[BUFSIZE];
extern char* word_map;
extern struct game_info game_info;