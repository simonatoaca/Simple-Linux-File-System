#include "list.h"

List *ll_create() {
	List *list = malloc(sizeof(*list));
	if(!list) {
		fprintf(stderr, "List malloc failed");
		return NULL;
	}
	list->head = 0;
	return list;
}

int ll_add_node(List *list, TreeNode *info) {

	if(!list) {
		fprintf(stderr, "No list :(");
		return 0;
	}

	if(ll_search(list, info->name)) {
		return 0;
	}

	if(!list->head) {
		list->head = calloc(1, sizeof(ListNode));
		list->head->info = info;
		list->head->next = NULL;
		return 1;
	}

	ListNode *next = list->head;
	ListNode *new = malloc(sizeof(*new));
	new->info = info;
	list->head = new;
	new->next = next;

	return 1;
}

ListNode *ll_remove_node(List *list, TreeNode *info) {

	if(!list) {
		fprintf(stderr, "No list :(");
		return NULL;
	}

	if(!list->head) {
		return NULL;
	}

	ListNode *curr = list->head;
	ListNode *prev = list->head;

	while(curr) {
		if(curr->info == info) {
			ListNode *next = curr->next;
			prev->next = next;
			return curr;
		}
		prev = curr;
		curr = curr->next;
	}

	return NULL;
}

void ll_print(List *list) {

	if(!list) {
		fprintf(stderr, "No list :(");
		return;
	}

	ListNode *curr = list->head;
	while(curr) {
		printf("%s\n", curr->info->name);
		curr = curr->next;
	}
}

ListNode *ll_search(List *list, char *node) {

	if(!list) {
		fprintf(stderr, "No list :(");
		return NULL;
	}
	ListNode *curr = list->head;
	while(curr) {
		if(!strcmp(curr->info->name, node)) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}