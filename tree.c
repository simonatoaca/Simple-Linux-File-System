#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."


FileTree *createFileTree(char* rootFolderName) {
	FileTree *file_tree =  malloc(sizeof(*file_tree));

	if (!file_tree) {
		fprintf(stderr, "Failed tree alloc\n");
		return NULL;
	}

	file_tree->root = malloc(sizeof(TreeNode));
	if (!file_tree->root) {
		fprintf(stderr, "Failed root alloc\n");
		free(file_tree);
		return NULL;
	}
	file_tree->root->name = strdup(rootFolderName);
	file_tree->root->parent = NULL;
	file_tree->root->type = FOLDER_NODE;
	file_tree->root->content = malloc(sizeof(FolderContent));
	((FolderContent *)(file_tree->root->content))->children = ll_create();
	
	return file_tree;
}

void freeTree(FileTree fileTree) {
    // TODO	
}


void ls(TreeNode* currentNode, char* arg) {

    if(!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}

	List *curr_list = ((FolderContent *)(currentNode->content))->children;
	if(!strcmp(arg, NO_ARG)) {
		ll_print(curr_list);
		return;
	}

	ListNode *curr = ll_search(curr_list, arg);
	if(curr) {
		printf("asta");
		curr_list = ((FolderContent *)curr->info->content)->children;
		ll_print(curr_list);
		return;
	}
}


void pwd(TreeNode* treeNode) {
    // TODO
}


TreeNode* cd(TreeNode* currentNode, char* path) {
    // TODO
}


void tree(TreeNode* currentNode, char* arg) {
    // TODO
}


void mkdir(TreeNode* currentNode, char* folderName) {

    if(!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}

	List *curr = ((FolderContent *)(currentNode->content))->children;
	TreeNode *info = calloc(1, sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(folderName);
	info->type = FOLDER_NODE;
	info->content = calloc(1, sizeof(FolderContent));
	((FolderContent *)info->content)->children = ll_create();
	

	if(!ll_add_node(curr, info)) {
		free(info->name);
		if(info->content)
			free(info->content);
		free(info);	
	} else {
		ll_print(curr);
	}
}


void rmrec(TreeNode* currentNode, char* resourceName) {
    // TODO
}


void rm(TreeNode* currentNode, char* fileName) {
    // TODO
}


void rmdir(TreeNode* currentNode, char* folderName) {
    // TODO
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
	if(!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}
	List *curr = ((FolderContent *)(currentNode->content))->children;
	TreeNode *info = calloc(1, sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(fileName);
	info->type = FILE_NODE;

	if(fileContent) {
		info->content = strdup(fileContent);
	}

	if(!ll_add_node(curr, info)) {
		free(info->name);
		if(info->content)
			free(info->content);
		free(info);	
	} else {
		ll_print(curr);
	}
}


void cp(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

void mv(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

