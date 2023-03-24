#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#include "game.h"
#include "list.h"
#include "common.h"
#include "player_data.h"

static atomic_bool timer_running = false;
static pthread_mutex_t printf_mutex;

static inline void term_erase_line(){
	printf("\033[K");
}

static inline void term_up_line(int n){
	printf("\033[%dA", n);
}

static inline void term_down_line(int n){
	printf("\033[%dB", n);
}

static inline void term_save_position(){
	printf("\033[s");
}

static inline void term_restore_position(){
	printf("\033[u");
}

word_node* game_init(){
	const int words = 3;
	int rand_word;
	word_node *head, *node;

	pthread_mutex_init(&printf_mutex, NULL);

	rand_word = rand() % NUM_OF_WORDS;
	head = ll_create_node(word_map + WORD_LENGTH*rand_word);

	for (int i = 0; i < words; i++){
		rand_word = rand() % NUM_OF_WORDS;
		node = ll_create_node(word_map + WORD_LENGTH*rand_word);
		ll_append(head, node);
	}

	return head;
}

static void* game_timer(__attribute__((unused))void* arg){
	struct timespec begin, current;

	clock_gettime(CLOCK_MONOTONIC, &begin);
	while (game_info.max_time - (current.tv_sec - begin.tv_sec) > 0){
		clock_gettime(CLOCK_MONOTONIC, &current);

		pthread_mutex_lock(&printf_mutex);
		term_save_position();
		term_up_line(2);
		printf("\r");
		term_erase_line();
		printf("Timer: %ld s", game_info.max_time - (current.tv_sec - begin.tv_sec));
		term_restore_position();
		fflush(stdout);
		pthread_mutex_unlock(&printf_mutex);

		usleep(100000);
	}
	timer_running = false;
	pthread_exit(NULL);
}

static void show_next_words(word_node* node){
	static const int words_display_number = 3;
	pthread_mutex_lock(&printf_mutex);
	term_save_position();
	term_up_line(1);
	term_erase_line();
	printf("-> %s <-", node->word);

	for (int i = 0; i < words_display_number; i++){
		node = node->next;
		printf(" %s ", node->word);
	}
	term_restore_position();
	fflush(stdout);
	pthread_mutex_unlock(&printf_mutex);
}

void game_loop(){
	pthread_t timer_thread;
	word_node* head = game_init();
	int score = 0;

	printf("You will have to correctly write as many words as possible\n");
	sleep(1);
	printf("Are you ready ?\n");
	sleep(1);
	printf("Go !\n");

	printf("\n\n");

	timer_running = true;
	pthread_create(&timer_thread, NULL, game_timer, NULL);
	while(1){
		show_next_words(head);

		if (fgets(buf, BUFSIZE, stdin) == NULL){
			perror("fgets");
			exit(EXIT_FAILURE);
		}
		buf[strlen(buf) - 1] = '\0';
		strtok(buf, " ");

		if (!timer_running)
			break;
		
		if (!strncmp(buf, head->word, strlen(head->word))){
			score++;
		}
		ll_pop(&head);
		pthread_mutex_lock(&printf_mutex);
		term_up_line(1); 
		term_erase_line(); //delete the user word
		fflush(stdout);
		pthread_mutex_unlock(&printf_mutex);

		ll_append(head, ll_create_node(word_map + WORD_LENGTH*(rand() % NUM_OF_WORDS)));
	}

	if (game_info.save == true)
		player_data_add_history(game_info.player_name, score / ((double)game_info.max_time/60));

	printf("\n\nGame finished, average : %f WPM\n", 60 * score / (float)game_info.max_time);
	ll_delete_list(head);
}