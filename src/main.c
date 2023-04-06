#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

#include <json-c/json_c_version.h>

#include "list.h"
#include "common.h"
#include "game.h"
#include "words.h"
#include "player_data.h"

#define REQUIRED_JSONC_VERSION 

char buf[BUFSIZE];
char* word_map;
struct game_info game_info = {.max_time = 30, .player_name = NULL, .save = false};

static bool strtol_check(long* val, char* str){
	char* endptr;
	errno = 0;    /* To distinguish success/failure after call */
	*val = strtol(str, &endptr, 10);

	/* Check for various possible errors */

	if ((errno == ERANGE && (*val == LONG_MAX || *val == LONG_MIN))
			|| (errno != 0 && *val == 0)) {
		perror("strtol");
		return false;
	}

	if (endptr == str) {
		fprintf(stderr, "No digits were found\n");
		return false;
	}

    return true;
}

void init(){
	size_t len;

	words_init();

	printf("Welcome to TypeSpeed !\n");
	printf("Type help for a list of commands\n");
	printf("Please enter your name\n");
	printf("~> ");
	fflush(stdout);

	if (fgets(buf, BUFSIZE, stdin) == NULL){
		exit(EXIT_FAILURE);
	}
	len = strlen(buf);
	buf[len - 1] = '\0';
	len--;

	game_info.player_name = malloc(len);
	memcpy(game_info.player_name, buf, len);

	while(1){
		printf("Do you want to save your scores ? y/n\n");
		printf("~> ");
		fflush(stdout);

		if (fgets(buf, BUFSIZE, stdin) == NULL){
			exit(EXIT_FAILURE);
		}
		buf[strlen(buf) - 1] = '\0';

		if (!strcmp(buf, "y") || !strcasecmp(buf, "yes")){
			game_info.save = true;
		} else if (!(!strcmp(buf, "n") || !strcasecmp(buf, "no"))){
			printf("Please answer by y(yes) or n(no)\n");
			continue;
		}
		break;
	}

	player_data_init();
}

void handle_input(){
	if (!strcmp(buf, "help")){
		printf("start: start game\n");
		printf("exit: exit game\n");
		printf("timer=<N>: set the time to <N> seconds\n");
		printf("average <name> [from] [to]: get the average words per minute\n"\
			"\t[from] and [to] are optional and the format is "\
			"dd-mm-yy|HH:MM:SS\n");
		printf("show_history <name>: prints the history of <name>\n");
		printf("delete_player <name>: deletes <name>'s history\n");
		printf("show_players: show name of all saved players in database\n");
	} else if (!strcmp(buf, "start")){
		game_loop();
	} else if (!strcmp(buf, "exit")){
		printf("Exiting game...\n");
		exit(EXIT_SUCCESS);
	} else if (!strncmp(buf, "timer=", 6)){
		long val;
		if (strtol_check(&val, buf + strlen("timer=")) == false){
			fprintf(stderr, "Error strtol, please write a valid number\n");
		} else {
			game_info.max_time = val;
		}
	} else if (!strncmp(buf, "average", 7)){
		char *name = NULL, *from = NULL, *to = NULL;
		char* token;
		int i = 0;
		token = strtok(buf, " ");
		while (token != NULL){
			token = strtok(NULL, " ");
			if (i == 0)
				name = token;
			else if (i == 1)
				from = token;
			else if (i == 2)
				to = token;
			i++;
		}
		double average = player_data_get_average(name, from, to);
		if (average >= 0){
			printf("Average for %s : %lf words per minute\n", name, average);
		}
	} else if (!strncmp(buf, "show_history", 12)){
		strtok(buf, " ");
		player_data_show_history(buf + strlen(buf) + 1);
	} else if (!strncmp(buf, "delete_player", 13)){
		char* name;
		strtok(buf, " ");
		name = buf + strlen(buf) + 1;
		if (player_data_delete_player(name))
			printf("Player %s was successfuly deleted from the database\n", name);
		else
			printf("Player %s was not found\n", name);
	} else if (!strcmp(buf, "show_players")){
		player_data_show_players();
	}
	else {
		printf("Error command not recongnized\n");
	}
}

int main(){
	if (json_c_version_num() < ((0 << 16) | (15 << 8) | 0)){
		fprintf(stderr, "minimum required version : json-c 0.16.0, your version : %s\n", json_c_version());
		return -1;
	}

	srand(time(NULL));
	init();

	while(1){
		printf("~> ");
		fflush(stdout);

		if (fgets(buf, BUFSIZE, stdin) == NULL){
			exit(EXIT_SUCCESS);
		}
		buf[strlen(buf) - 1] = '\0';

		handle_input();
	}
}
