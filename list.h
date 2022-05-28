#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/*
	@brief creates an empty linked list
    @return return the newly created list
*/
List *ll_create();

/*
	@brief adds a new node to the start of the list
    @param list - the list
	@param info - the info to be added as a new node
    @return 1 if the node didn't already exist, 0 otherwise
*/
int ll_add_node(List *list, TreeNode *info);

/*
	@brief removes <node_name> from a list
    @param list - the list
	@param node_name - the name of the node to be removed
    @return the removed node, so it is freed
*/
ListNode *ll_remove_node(List *list, char *node_name);

/*
	@brief prints the list's elements
    @param list - the list
*/
void ll_print(List *list);

/*
	@brief searches for <node> in list
    @param list - the list
	@param node - the node that is being searched
    @return the node if it is found, NULL otherwise
*/
ListNode *ll_search(List *list, char *node);

/*
	@brief frees the list
    @param list - the list
	@param free_data - custom free function for the nodes
*/
void ll_free(List *list, void(*free_data)(ListNode *));
#endif
