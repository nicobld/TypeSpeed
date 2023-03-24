#pragma once
#include <time.h>
#include <stdbool.h>

#include "list.h"

struct game_info {
	time_t max_time;
	char* player_name;
	bool save;
};

void game_loop();
word_node* game_init();