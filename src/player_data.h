#pragma once
#include <stdbool.h>

void player_data_show_players();
bool player_data_delete_player(char* name);
void player_data_show_history(char* name);
double player_data_get_average(char* name, char* from, char* to);
void player_data_add_history(char* name, double ave);
void player_data_init();