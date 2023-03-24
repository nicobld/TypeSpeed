#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include <json-c/json.h>

#include "player_data.h"
#include "game.h"
#include "common.h"

#define DATA_FILE_NAME "data/player_info.json"

static json_object* root;
static json_object* players;

static json_object* player_data_get_player(char* name){
	int json_len = json_object_array_length(players);
	json_object *player, *player_name;
	const char* string_name;

	for (int i = 0; i < json_len; i++){
		player = json_object_array_get_idx(players, i);
		player_name = json_object_object_get(player, "name");
		string_name = json_object_get_string(player_name);
		if (!strcmp(string_name, name)){
			return player;
		}
	}
	return NULL;
}

static void player_data_create(){

	root = json_object_new_object();
	players = json_object_new_array_ext(0);
	json_object_object_add(root, "players", players);
}

void player_data_show_players(){
	json_object* player;
	json_object* json_name;
	const char* string_name;

	printf("Saved players names : ");

	int len = json_object_array_length(players);
	for (int i = 0; i < len; i++){
		player = json_object_array_get_idx(players, i);
		json_name = json_object_object_get(player, "name");
		string_name = json_object_get_string(json_name);

		printf("%s", string_name);
		if (i < len - 1)
			printf(", ");
	}
	printf("\n");
	json_object_to_file_ext(DATA_FILE_NAME, root, JSON_C_TO_STRING_PRETTY);
}

bool player_data_delete_player(char* name){
	json_object* player;
	json_object* json_name;
	const char* string_name;

	int len = json_object_array_length(players);
	int i;
	for (i = 0; i < len; i++){
		player = json_object_array_get_idx(players, i);
		json_name = json_object_object_get(player, "name");
		string_name = json_object_get_string(json_name);

		if (!strcmp(string_name, name)){
			json_object_array_del_idx(players, i, 1);
			break;
		}
	}
	json_object_to_file_ext(DATA_FILE_NAME, root, JSON_C_TO_STRING_PRETTY);
	return i < len;
}

void player_data_show_history(char* name){
	json_object* player = player_data_get_player(name);

	if (player == NULL){
		printf("Player not found\n");
		return;
	}

	json_object* history = json_object_object_get(player, "history");
	printf("%s\'s history : %s\n", name, json_object_to_json_string_ext(history, JSON_C_TO_STRING_PRETTY));
}

double player_data_get_average(char* name, char* from, char* to){
	json_object* player = player_data_get_player(name);
	double average = 0;
	struct tm tm_from, tm_to;
	time_t time_from, time_to;
	if (from != NULL){
		sscanf(from, "%u-%u-%u|%u:%u:%u", &tm_from.tm_mday, &tm_from.tm_mon, &tm_from.tm_year, &tm_from.tm_hour, &tm_from.tm_min, &tm_from.tm_sec);
		tm_from.tm_mon -= 1;
		tm_from.tm_year += 2000 - 1900;
		tm_from.tm_isdst = -1;
		time_from = mktime(&tm_from);
	} else {
		time_from = 0;
	}
	if (to != NULL){
		sscanf(to, "%u-%u-%u|%u:%u:%u", &tm_to.tm_mday, &tm_to.tm_mon, &tm_to.tm_year, &tm_to.tm_hour, &tm_to.tm_min, &tm_to.tm_sec);
		tm_to.tm_mon -= 1;
		tm_to.tm_year += 2000 - 1900;
		tm_to.tm_isdst = -1;
		time_to = mktime(&tm_to);
	} else {
		time_to = LONG_MAX;
	}

	if (player == NULL){
		printf("Player not found\n");
		return -1;
	}


	json_object* history = json_object_object_get(player, "history");
	json_object* history_value;
	json_object* json_average;
	json_object* json_date;
	const char* date;
	int len = json_object_array_length(history);
	int count = 0;
	struct tm tm_date;
	time_t time_date;
	for (int i = 0; i < len; i++){
		history_value = json_object_array_get_idx(history, i);
		json_date = json_object_object_get(history_value, "date");
		date = json_object_get_string(json_date);

		sscanf(date, "%u-%u-%u|%u:%u:%u", &tm_date.tm_mday, &tm_date.tm_mon, &tm_date.tm_year, &tm_date.tm_hour, &tm_date.tm_min, &tm_date.tm_sec);
		tm_date.tm_mon -= 1;
		tm_date.tm_year += 2000 - 1900;
		tm_date.tm_isdst = -1;
		time_date = mktime(&tm_date);
		if (time_date > time_from && time_date < time_to){
			json_average = json_object_object_get(history_value, "average");
			average += json_object_get_double(json_average);
			count++;
		}
	}
	return average / count;
}


void player_data_add_history(char* name, double ave){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char s[64];
	strftime(s, 64, "%d-%m-%y|%T", &tm);

	json_object* player = player_data_get_player(name);
	if (player == NULL){
		fprintf(stderr, "Error player %s could not be found\n", name);
		return;
	}
	json_object* history = json_object_object_get(player, "history");

	json_object* date = json_object_new_string(s);
	json_object* average = json_object_new_double(ave);
	json_object* history_value = json_object_new_object();
	json_object_object_add(history_value, "date", date);
	json_object_object_add(history_value, "average", average);

	json_object_array_add(history, history_value);

	json_object_to_file_ext(DATA_FILE_NAME, root, JSON_C_TO_STRING_PRETTY);
}

void player_data_init(){
	int fd;
	char mode[] = "0666";
	unsigned long imode;
	
	if ((fd = open(DATA_FILE_NAME, O_RDWR | O_CREAT, 0x1B6)) == -1){
		perror("open " DATA_FILE_NAME);
		exit(EXIT_FAILURE);
	}
	imode = strtoul(mode, NULL, 8);
	if (fchmod(fd, imode) == -1){
		perror("chmod");
		exit(EXIT_FAILURE);
	}
	close(fd);

	root = json_object_from_file(DATA_FILE_NAME);

	if (root == NULL){
		printf("No json file ... Creating new json\n");
		player_data_create();
	}

	players = json_object_object_get(root, "players");
	json_object* player = player_data_get_player(game_info.player_name);

	if (player == NULL && game_info.save == true){
		player = json_object_new_object();
		json_object_object_add(player, "name", json_object_new_string(game_info.player_name));
		json_object_object_add(player, "history", json_object_new_array_ext(0));
		json_object_array_add(players, player);
	}

	// printf("%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
	json_object_to_file_ext(DATA_FILE_NAME, root, JSON_C_TO_STRING_PRETTY);
	// json_object_put(root);
}