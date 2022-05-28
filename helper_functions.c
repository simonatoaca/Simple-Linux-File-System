#include "helper_functions.h"

void __free_helper(TreeNode *tree_node) {
	if (!tree_node)
		return;

	List *list = NULL;
	if (tree_node->type == FOLDER_NODE) {
		list = ((FolderContent *)(tree_node->content))->children;
	}

	// The dir is empty
	if (!list->head) {
		rmdir(tree_node->parent, tree_node->name);
		return;
	}

	// The dir is not empty -> go through its contents
	// Empty dirs and files are "leaves" in the tree -> free them directly
	// Go recursively until "leaves" are reached
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

void __tree_helper(TreeNode *tree_node, int *level, int *dirs, int *files) {
	List *children = ((FolderContent *)tree_node->content)->children;
	ListNode *curr_node = children->head;

	while (curr_node) {
		if (curr_node->info->type == FILE_NODE) {
			(*files)++;
			printf("%*s\n", *level * TREE_CMD_INDENT_SIZE, curr_node->info->name);
		} else {
			(*dirs)++;
			printf("%*s\n", *level * TREE_CMD_INDENT_SIZE, curr_node->info->name);
			int new_level = *level + 1;
			__tree_helper(curr_node->info, &new_level, dirs, files);
		}
		curr_node = curr_node->next;
	}
}

int get_file_info(TreeNode *currentNode, ListNode *source_node, char *source,
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

		// Get path (without the source itself)
		char *truncated_path = calloc(1, strlen(source) - len + 1);
		strncpy(truncated_path, source, strlen(source) - len);

		// Folow the path and get the source info
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

int get_folder_info(TreeNode *currentNode, ListNode *source_node, char *source,
				  char **folder_name, List **folder_content) {
	if(source_node) {
		// The source folder is in the current dir
		*folder_name = strdup(source_node->info->name);
		*folder_content = ((FolderContent *)source_node->info->content)->children;
	} else {
		char *folder_name_aux = strrchr(source, '/');
		int len = strlen(folder_name_aux);

		// Get path (without the source itself)
		char *truncated_path = calloc(1, strlen(source) - len + 1);
		strncpy(truncated_path, source, strlen(source) - len);

		// Folow the path and get the source info
		TreeNode *parent_node = cd(currentNode, truncated_path);
		List *list = ((FolderContent *)(parent_node->content))->children;
		ListNode *child = ll_search(list, folder_name_aux + 1);

		// Check that the source exists in the path
		if (!child) {
			printf("mv: failed to access '%s': Not a directory", source);
			free(truncated_path);
		}

		*folder_name = strdup(child->info->name);
		*folder_content = ((FolderContent *)child->info->content)->children;
		free(truncated_path);
	}
	return 1;
}

void mv_rec(TreeNode *currentNode, char *source, char *destination) {
	char *folder_name = NULL;
	List *folder_content = NULL;

	// Because it is modified by strtok
	char *aux_dest = strdup(destination);

	// Check if the source is in the current dir
	List *list = ((FolderContent *)(currentNode->content))->children;
	ListNode *source_node = ll_search(list, source);
	if (!get_folder_info(currentNode, source_node, source,
		&folder_name, &folder_content)) {
		free(aux_dest);
		return;
	}

	// Flag to see if the mv has been done -> free aux strings
	int done_mv = 0;

	TreeNode *next_dir = currentNode;
	list = ((FolderContent *)(currentNode->content))->children;
	char *next_directory = strtok(aux_dest, "/");

	while (next_directory) {
		if (!strcmp(next_directory, PARENT_DIR)) {
			next_dir = next_dir->parent;
			if (!next_dir)	{
				printf("mv: failed to access '%s': Not a directory", destination);
				done_mv = 1;
				break;
			}
		} else {
			ListNode *child = ll_search(list, next_directory);
			if (!child) {
				printf("mv: failed to access '%s': Not a directory", destination);
				done_mv = 1;
				break;
			}

			if(child->info->type == FOLDER_NODE) {
				// The next directory is a folder
				next_dir = child->info;
			}
		}
		list = ((FolderContent *)next_dir->content)->children;
		next_directory = strtok(NULL, "/");
	}

	if (!done_mv) {
		// Create new folder identical to the source:
		mkdir(next_dir, folder_name);
		ListNode *dest_list = ll_search(((FolderContent *)
		next_dir->content)->children, folder_name);

		// Copy the content of the source folder to the new one
		ListNode *node = folder_content->head;
		// Visit the contents of the source folder
		while (node) {
			// For files, copy the content
			if(node->info->type == FILE_NODE) {
				char *file_name = strdup(node->info->name);
				char *file_content = strdup(((FileContent *)node->info->content)->text);
				touch(dest_list->info, file_name, file_content);
			} else {
				// For folders, create a new folder
				char *folder_name = strdup(node->info->name);
				mkdir(dest_list->info, folder_name);
			}
			node = node->next;
		}
	}

	free(aux_dest);
}

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
