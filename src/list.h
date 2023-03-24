#pragma once

typedef struct word_node {
	char* word;
	struct word_node* next;
} word_node;


void ll_delete_list(word_node* head);
void ll_print(word_node* head);
word_node* ll_create_node(char* string);
void ll_append(word_node* head, word_node* node);
//Pops the first node and modifies head to the next node
void ll_pop(word_node** head);