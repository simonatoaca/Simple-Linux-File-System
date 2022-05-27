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
	free(rootFolderName);

	return file_tree;
}

void __free_helper(TreeNode *tree_node) {
	if (!tree_node)
		return;

	List *list = NULL;
	if (tree_node->type == FOLDER_NODE) {
		list = ((FolderContent *)(tree_node->content))->children;
	}

	if (!list)
		return;

	// The dir is empty
	if (!list->head) {
		rmdir(tree_node->parent, tree_node->name);
		return;
	}

	ListNode *curr_node = list->head;
	while (curr_node) {
		ListNode *aux_node = curr_node->next;
		if (curr_node->info->type == FOLDER_NODE) {
			if (!((FolderContent *)(curr_node->info->content))->children->head) {
				rmdir(curr_node->info->parent, curr_node->info->name);
			} else {
				__free_helper(curr_node->info);
			}
		} else {
			rm(curr_node->info->parent, curr_node->info->name);
		}
		curr_node = aux_node;
	}
	return;
}

void freeTree(FileTree *fileTree) {
	if (fileTree->root) {
		if (((FolderContent *)fileTree->root->content)->children->head)
			__free_helper(fileTree->root);

		free(fileTree->root->name);
		ll_free(((FolderContent *)fileTree->root->content)->children, custom_free);
		free(fileTree->root->content);
		free(fileTree->root);
	}
	free(fileTree);
}

void ls(TreeNode* currentNode, char* arg) {
	if (!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}

	List *curr_list = ((FolderContent *)(currentNode->content))->children;
	if (!strcmp(arg, NO_ARG)) {
		ll_print(curr_list);
		return;
	}

	ListNode *curr = ll_search(curr_list, arg);

	if (!curr) {
		printf("ls: cannot access '%s': No such file or directory", arg);
		return;
	}

	if (curr->info->type == FILE_NODE) {
		printf("%s: %s\n", curr->info->name, ((FileContent *)
		curr->info->content)->text);
		return;
	}

	if (curr->info->type == FOLDER_NODE) {
		curr_list = ((FolderContent *)curr->info->content)->children;
		ll_print(curr_list);
		return;
	}
}

void pwd(TreeNode* treeNode) {
	char path[LINE_MAX_LEN] = "";
    TreeNode *prev_dir = treeNode;

	while (prev_dir) {
		char *aux = strdup(path);
		sprintf(path, "%s/%s", prev_dir->name, aux);
		prev_dir = prev_dir->parent;
		free(aux);
	}

	path[strlen(path) - 1] = '\0';
	printf("%s", path);
}

TreeNode* cd(TreeNode* currentNode, char* path) {
	TreeNode *next_dir = currentNode;
	List *curr_list = ((FolderContent *)(currentNode->content))->children;

	// Because the string pointer is modified by strtok
	char *aux_path = strdup(path);
	char *next_directory = strtok(aux_path, "/");

	while (next_directory) {
		if (!strcmp(next_directory, PARENT_DIR)) {
			next_dir = next_dir->parent;
			if (!next_dir) {
				free(aux_path);
				return currentNode;
			}
		} else {
			ListNode *child = ll_search(curr_list, next_directory);
			if (child) {
				next_dir = child->info;
			} else {
				printf("cd: no such file or directory: %s", path);
				free(aux_path);
				return currentNode;
			}
		}
		curr_list = ((FolderContent *)next_dir->content)->children;
		next_directory = strtok(NULL, "/");
	}
	free(aux_path);
	return next_dir;
}

void tree(TreeNode* currentNode, char* arg) {
	if (!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}
}

void mkdir(TreeNode *currentNode, char *folderName) {
    if (!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}

	List *curr = ((FolderContent *)(currentNode->content))->children;

	if(ll_search(curr, folderName)) {
		printf("mkdir: cannot create directory '%s': File exists", folderName);
		return;
	}

	TreeNode *info = calloc(1, sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(folderName);
	info->type = FOLDER_NODE;
	info->content = calloc(1, sizeof(FolderContent));
	((FolderContent *)info->content)->children = ll_create();

	if (!ll_add_node(curr, info)) {
		free(info->name);
		ll_free(((FolderContent *)info->content)->children, custom_free);
		free(info->content);
		free(info);
	}
	free(folderName);
}

void rmrec(TreeNode *currentNode, char *resourceName) {
    List *list = ((FolderContent *)(currentNode->content))->children;
    ListNode *removed_node = ll_search(list, resourceName);
	if (!removed_node) {
		printf("rmrec: failed to remove '%s': No such file or directory",
		resourceName);
		return;
	}
	if (removed_node->info->type == FOLDER_NODE) {
		if (((FolderContent *)(removed_node->info->content))->children->head) {
			ll_free(((FolderContent *)(removed_node->info->content))->children,
					custom_free);
			((FolderContent *)(removed_node->info->content))->children = ll_create();
		}

		rmdir(currentNode, resourceName);
		return;
	}

	rm(currentNode, resourceName);
}

void rm(TreeNode *currentNode, char *fileName) {
	List *list = ((FolderContent *)(currentNode->content))->children;
    ListNode *removed_node = ll_search(list, fileName);
	if (!removed_node) {
		printf("rm: failed to remove '%s': No such file or directory", fileName);
		return;
	}

	if (removed_node->info->type != FILE_NODE) {
		printf("rm: cannot remove '%s': Is a directory", fileName);
		return;
	}

	removed_node = ll_remove_node(list, fileName);
	custom_free(removed_node);
}

void rmdir(TreeNode* currentNode, char* folderName) {
    List *list = ((FolderContent *)(currentNode->content))->children;
	if (!list) {
		return;
	}
    ListNode *removed_node = ll_search(list, folderName);
	if (!removed_node) {
		printf("rmdir: failed to remove '%s': No such file or directory", folderName);
		return;
	}

	if (removed_node->info->type != FOLDER_NODE) {
		printf("rmdir: failed to remove '%s': Not a directory", folderName);
		return;
	}

	if (((FolderContent *)(removed_node->info->content))->children->head) {
		printf("rmdir: failed to remove '%s': Directory not empty", folderName);
		return;
	}

	removed_node = ll_remove_node(list, folderName);
	custom_free(removed_node);
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent) {
	if (!currentNode) {
		fprintf(stderr, "No current node :(");
		return;
	}

	List *curr = ((FolderContent *)(currentNode->content))->children;

	if (ll_search(curr, fileName)) {
		return;
	}

	TreeNode *info = calloc(1, sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(fileName);
	info->type = FILE_NODE;

	if (fileContent) {
		info->content = calloc(1, sizeof(fileContent));
		((FileContent *)info->content)->text = strdup(fileContent);
	}

	if (!ll_add_node(curr, info)) {
		free(info->name);
		if(info->content)
			free(info->content);
		free(info);
	}

	free(fileName);
	free(fileContent);
}

int get_source_info(TreeNode *currentNode, ListNode *source_node, char *source,
				  char **file_name, char **file_content) {
	if (source_node) {
		if (source_node->info->type == FOLDER_NODE) {
			printf("cp: -r not specified; omitting directory '%s'", source);
			return 0;
		}

		// The source is in the current dir and is a file
		*file_name = strdup(source_node->info->name);
		*file_content = strdup(((FileContent *)source_node->info->content)->text);
	} else {
		char *file_name_aux = strrchr(source, '/');
		int len = strlen(file_name_aux);
		char *truncated_path = calloc(1, strlen(source) - len + 1);
		strncpy(truncated_path, source, strlen(source) - len);

		TreeNode *path = cd(currentNode, truncated_path);
		List *list = ((FolderContent *)(path->content))->children;
		ListNode *file = ll_search(list, file_name_aux + 1);

		if (file && file->info->type == FOLDER_NODE) {
			printf("cp: -r not specified; omitting directory '%s'", source);
			free(truncated_path);
			return 0;
		}

		*file_name = strdup(file->info->name);
		*file_content = strdup(((FileContent *)file->info->content)->text);
		free(truncated_path);
	}
	return 1;
}

void cp(TreeNode *currentNode, char *source, char *destination) {
	char *file_name = NULL, *file_content = NULL;
	// Because it is modified by strtok
	char *aux_dest = strdup(destination);

	// Check if the source is in the current dir
	List *list = ((FolderContent *)(currentNode->content))->children;
	ListNode *source_node = ll_search(list, source);
	if (!get_source_info(currentNode, source_node, source,
		&file_name, &file_content)) {
		free(aux_dest);
		return;
	}

	// Flag to see if the cp has been done -> free aux strings
	int done_cp = 0;

	TreeNode *next_dir = currentNode;
	list = ((FolderContent *)(currentNode->content))->children;
	char *next_directory = strtok(aux_dest, "/");

	while (next_directory) {
		if (!strcmp(next_directory, PARENT_DIR)) {
			next_dir = next_dir->parent;
			if (!next_dir)	{
				printf("cp: failed to access '%s': Not a directory", destination);
				done_cp = 1;
				break;
			}
		} else {
			ListNode *child = ll_search(list, next_directory);
			if (!child) {
				printf("cp: failed to access '%s': Not a directory", destination);
				done_cp = 1;
				break;
			}

			if (child->info->type == FOLDER_NODE) {
				next_dir = child->info;
			} else {
				// The destination is a file -> replacing its content
				if (((FileContent *)child->info->content)->text)
					free(((FileContent *)child->info->content)->text);
				((FileContent *)child->info->content)->text = strdup(file_content);

				done_cp = 1;
				break;
			}
		}
		list = ((FolderContent *)next_dir->content)->children;
		next_directory = strtok(NULL, "/");
	}

	if (!done_cp) {
		// Create new file identical to the source:
		touch(next_dir, file_name, file_content);
	}
	free(aux_dest);
}

void mv(TreeNode* currentNode, char* source, char* destination) {
	List *list = ((FolderContent *)(currentNode->content))->children;
	ListNode *source_node = ll_search(list, source);
	
	cp(currentNode, source, destination);
	if(source_node->info->type == FOLDER_NODE) {
		rmrec(source_node->info, source);
	} else {
		rm(source_node->info, source);
	}
}

// duplicat
TreeNode* move_to(TreeNode* currentNode, char* path) {
	TreeNode *next_dir = currentNode;
	List *curr_list = ((FolderContent *)(currentNode->content))->children;

	// Because the string pointer is modified by strtok
	char *aux_path = strdup(path);
	char *next_directory = strtok(aux_path, "/");

	while (next_directory) {
		if (!strcmp(next_directory, PARENT_DIR)) {
			next_dir = next_dir->parent;
			if (!next_dir) {
				free(aux_path);
				return currentNode;
			}
		} else {
			ListNode *child = ll_search(curr_list, next_directory);
			if (child) {
				next_dir = child->info;
			} else {
				free(aux_path);
				return NULL;
			}
		}
		curr_list = ((FolderContent *)next_dir->content)->children;
		next_directory = strtok(NULL, "/");
	}

	free(aux_path);
	return next_dir;
}
