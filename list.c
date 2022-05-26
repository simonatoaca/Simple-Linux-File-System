#include "list.h"

void custom_free(ListNode *curr) {
	if (curr->info->type == FILE_NODE)
		free(((FileContent *)curr->info->content)->text);
	else
		ll_free(((FolderContent *)curr->info->content)->children, custom_free);
	
	if (curr && curr->info && curr->info->content)
		free(curr->info->content);
	if (curr && curr->info) {
		free(curr->info->name);
		free(curr->info);
		free(curr);
	}
}


List *ll_create() {
	List *list = malloc(sizeof(*list));
	if (!list) {
		fprintf(stderr, "List malloc failed");
		return NULL;
	}
	list->head = 0;
	return list;
}

int ll_add_node(List *list, TreeNode *info) {

	if (!list) {
		fprintf(stderr, "No list :(");
		return 0;
	}

	if (ll_search(list, info->name)) {
		return 0;
	}

	ListNode *next = list->head;
	ListNode *new = malloc(sizeof(*new));
	new->info = info;
	list->head = new;
	new->next = next;

	return 1;
}

ListNode *ll_remove_node(List *list, char *node_name) {

	if (!list) {
		fprintf(stderr, "No list :(");
		return NULL;
	}

	if (!list->head) {
		return NULL;
	}

	if (!list->head->next) {
		ListNode *curr = list->head;
		list->head = NULL;
		return curr;
	}

	ListNode *curr = list->head;
	ListNode *prev = NULL;

	while (curr) {
		if (!strcmp(curr->info->name, node_name)) {
			if (prev)
				prev->next = curr->next;
			else
				list->head = curr->next;
			return curr;
		}
		prev = curr;
		curr = curr->next;
	}

	return NULL;
}

void ll_print(List *list) {

	if (!list) {
		fprintf(stderr, "No list :(");
		return;
	}

	ListNode *curr = list->head;
	while (curr) {
		printf("%s\n", curr->info->name);
		curr = curr->next;
	}
}

ListNode *ll_search(List *list, char *node) {

	if (!list) {
		fprintf(stderr, "No list :(");
		return NULL;
	}
	ListNode *curr = list->head;
	while (curr) {
		if (!strcmp(curr->info->name, node)) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

void ll_free(List *list, void(*free_data)(ListNode *)) {
	if (!list) {
		fprintf(stderr, "No list :(");
		return;
	}

	ListNode *curr = list->head;
	while (curr) {
		ListNode *aux = curr;
		curr = curr->next;
		free_data(aux);
	}

	free(list);	
}