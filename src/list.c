#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "list.h"

void ll_delete_list(word_node* head){
	if (head == NULL) return;
	for (word_node* node = head; node->next != NULL;){
		ll_pop(&node);
	}
}

void ll_print(word_node* head){
	printf("Print list : ");
	if (head == NULL) return;
	for (word_node* node = head; node->next != NULL; node = node->next){
		printf("%s, ", node->word);
	}
	printf("\n");
}

word_node* ll_create_node(char* string){
	size_t len;
	word_node* node;

	len = strlen(string) + 1;
	node = malloc(sizeof(word_node));
	node->next = NULL;
	node->word = malloc(len * sizeof(char));

	memcpy(node->word, string, len);

	return node;
}

void ll_append(word_node* head, word_node* node){
	word_node* cur;
	if (head == NULL) return;

	for (cur = head; cur->next != NULL; cur = cur->next);
	cur->next = node;
}

void ll_pop(word_node** head){
	word_node* save_head;
	save_head = *head;

	if (save_head->next != NULL)
		*head = save_head->next;
	else
		*head = NULL;

	free(save_head->word);
	free(save_head);
}