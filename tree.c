#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "list.h"
#include "helper_functions.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

FileTree *createFileTree(char* rootFolderName) {
	FileTree *file_tree =  malloc(sizeof(*file_tree));

	if (!file_tree) {
		fprintf(stderr, "Failed tree alloc\n");
		return NULL;
	}

	// Allocate memory for the root
	file_tree->root = malloc(sizeof(TreeNode));
	if (!file_tree->root) {
		fprintf(stderr, "Failed root alloc\n");
		free(file_tree);
		return NULL;
	}

	// initialize root
	file_tree->root->name = rootFolderName;
	file_tree->root->parent = NULL;
	file_tree->root->type = FOLDER_NODE;
	file_tree->root->content = malloc(sizeof(FolderContent));
	((FolderContent *)(file_tree->root->content))->children = ll_create();

	return file_tree;
}

void freeTree(FileTree *fileTree) {
	// Check if there is something to free other that the root
	if (((FolderContent *)fileTree->root->content)->children->head)
		__free_helper(fileTree->root);

	// Free root
	free(fileTree->root->name);
	ll_free(((FolderContent *)fileTree->root->content)->children, custom_free);
	free(fileTree->root->content);
	free(fileTree->root);

	// Free tree
	free(fileTree);
}

void ls(TreeNode* currentNode, char* arg) {
	// Get dir content
	List *curr_list = ((FolderContent *)(currentNode->content))->children;

	// No args -> Print current directory's content
	if (!strcmp(arg, NO_ARG)) {
		ll_print(curr_list);
		return;
	}

	// Arg present
	ListNode *curr = ll_search(curr_list, arg);

	if (!curr) {
		printf("ls: cannot access '%s': No such file or directory", arg);
		return;
	}

	// If the arg is a file -> print the text inside
	if (curr->info->type == FILE_NODE) {
		printf("%s: %s\n", curr->info->name, ((FileContent *)
		curr->info->content)->text);
		return;
	}

	// If the arg is a folder -> print its content
	if (curr->info->type == FOLDER_NODE) {
		curr_list = ((FolderContent *)curr->info->content)->children;
		ll_print(curr_list);
		return;
	}
}

void pwd(TreeNode* treeNode) {
	char path[LINE_MAX_LEN] = "";
    TreeNode *prev_dir = treeNode;

	// Move backwards until root and construct the path
	while (prev_dir) {
		char *aux = strdup(path);
		sprintf(path, "%s/%s", prev_dir->name, aux);
		prev_dir = prev_dir->parent;
		free(aux);
	}

	// Chop off the extra '/'
	path[strlen(path) - 1] = '\0';
	printf("%s", path);
}

TreeNode* cd(TreeNode* currentNode, char* path) {
	TreeNode *next_dir = currentNode;
	List *curr_list = ((FolderContent *)(currentNode->content))->children;

	// Because the string pointer is modified by strtok
	char *aux_path = strdup(path);

	// Parse the path and move from dir tot dir
	char *next_directory = strtok(aux_path, "/");

	while (next_directory) {
		if (!strcmp(next_directory, PARENT_DIR)) {
			// The path moves backwards
			next_dir = next_dir->parent;
			if (!next_dir) {
				free(aux_path);
				return currentNode;
			}
		} else {
			// Go forward if possible
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
	if (strcmp(arg, NO_ARG)) {
		TreeNode *next_dir = currentNode;
		List *curr_list = ((FolderContent *)(currentNode->content))->children;

		// Because the string pointer is modified by strtok
		char *aux_path = strdup(arg);
		char *next_directory = strtok(aux_path, "/");

		// Do what cd does (follow the path) but with other error messages
		while (next_directory) {
			if (!strcmp(next_directory, PARENT_DIR)) {
				next_dir = next_dir->parent;
				if (!next_dir) {
					free(aux_path);
					break;
				}
			} else {
				ListNode *child = ll_search(curr_list, next_directory);
				if (child) {
					next_dir = child->info;
					if (child->info->type == FILE_NODE) {
						printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
						free(aux_path);
						return;
					}
				} else {
					printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
					free(aux_path);
					return;
				}
			}
			curr_list = ((FolderContent *)next_dir->content)->children;
			next_directory = strtok(NULL, "/");
		}

		free(aux_path);
		currentNode = next_dir;
	}

	// Go recursively through the tree, the current level being 0
	int level = 0, dirs = 0, files = 0;
	__tree_helper(currentNode, &level, &dirs, &files);
	printf("%d directories, %d files\n", dirs, files);
}

void mkdir(TreeNode *currentNode, char *folderName) {
	// Current dir's contents
	List *curr = ((FolderContent *)(currentNode->content))->children;

	// Verify if <folderName> already exists
	if (ll_search(curr, folderName)) {
		printf("mkdir: cannot create directory '%s': File exists", folderName);
		return;
	}

	// Initialize a new folder
	TreeNode *info = malloc(sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(folderName);
	info->type = FOLDER_NODE;
	info->content = malloc(sizeof(FolderContent));
	((FolderContent *)info->content)->children = ll_create();

	// Add the new folder to the current folder's contents
	ll_add_node(curr, info);
}

void rmrec(TreeNode *currentNode, char *resourceName) {
	// Get current folder's contents
    List *list = ((FolderContent *)(currentNode->content))->children;

	// Verify if <resourceName> exists
    ListNode *removed_node = ll_search(list, resourceName);
	if (!removed_node) {
		printf("rmrec: failed to remove '%s': No such file or directory",
				resourceName);
		return;
	}

	// The node to be removed is a folder-> free its contents and then
	// remove the folder from the current node's contents
	if (removed_node->info->type == FOLDER_NODE) {
		if (((FolderContent *)(removed_node->info->content))->children->head) {
			ll_free(((FolderContent *)(removed_node->info->content))->children,
					custom_free);
			((FolderContent *)(removed_node->info->content))->children = ll_create();
		}

		rmdir(currentNode, resourceName);
		return;
	}

	// The node to be removed is a file
	rm(currentNode, resourceName);
}

void rm(TreeNode *currentNode, char *fileName) {
	// Get current folder's contents
	List *list = ((FolderContent *)(currentNode->content))->children;

	// Verify if <fileName> exists
    ListNode *removed_node = ll_search(list, fileName);
	if (!removed_node) {
		printf("rm: failed to remove '%s': No such file or directory", fileName);
		return;
	}

	// Verify if <fileName> is a file
	if (removed_node->info->type != FILE_NODE) {
		printf("rm: cannot remove '%s': Is a directory", fileName);
		return;
	}

	// Remove and free the file
	removed_node = ll_remove_node(list, fileName);
	custom_free(removed_node);
}

void rmdir(TreeNode* currentNode, char* folderName) {
	// Get current folder's contents
    List *list = ((FolderContent *)(currentNode->content))->children;

	// Verify if <folderName> exists
    ListNode *removed_node = ll_search(list, folderName);
	if (!removed_node) {
		printf("rmdir: failed to remove '%s': No such file or directory", folderName);
		return;
	}

	// Verify if <folderName> is a folder
	if (removed_node->info->type != FOLDER_NODE) {
		printf("rmdir: failed to remove '%s': Not a directory", folderName);
		return;
	}

	// Verify if <folderName> is empty
	if (((FolderContent *)(removed_node->info->content))->children->head) {
		printf("rmdir: failed to remove '%s': Directory not empty", folderName);
		return;
	}

	// Remove and free the folder
	removed_node = ll_remove_node(list, folderName);
	custom_free(removed_node);
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent) {
	// Get current folder's contents
	List *curr = ((FolderContent *)(currentNode->content))->children;

	// Verify if <fileName> already exists
	if (ll_search(curr, fileName))
		return;

	// Initialize a new file
	TreeNode *info = calloc(1, sizeof(*info));
	info->parent = currentNode;
	info->name = strdup(fileName);
	info->type = FILE_NODE;

	if (fileContent) {
		info->content = calloc(1, sizeof(fileContent));
		((FileContent *)info->content)->text = strdup(fileContent);
	}

	// Add file to directory
	ll_add_node(curr, info);

	free(fileName);
	free(fileContent);
}

void cp(TreeNode *currentNode, char *source, char *destination) {
	char *file_name = NULL, *file_content = NULL;

	// Because <destination> is modified by strtok
	char *aux_dest = strdup(destination);

	// Check if the source is in the current dir
	List *list = ((FolderContent *)(currentNode->content))->children;
	ListNode *source_node = ll_search(list, source);

	// Get source file info
	if (!get_file_info(currentNode, source_node, source,
		&file_name, &file_content)) {
		free(aux_dest);
		return;
	}

	// Flag to see if cp has been done -> free aux strings
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
	TreeNode *source_dir = cd(currentNode, source);

	if(source_dir->type == FOLDER_NODE) {
		mv_rec(currentNode, source, destination);
		rmrec(currentNode, source);
	} else {
		// The source is a file
		char *file_name = NULL, *file_content = NULL;
		// Because <destination> is modified by strtok
		char *aux_dest = strdup(destination);

		// Check if the source is in the current dir
		List *list = ((FolderContent *)(currentNode->content))->children;
		ListNode *source_node = ll_search(list, source);
		if (!get_file_info(currentNode, source_node, source,
			&file_name, &file_content)) {
			free(aux_dest);
			return;
		}
		// Removes the source file after extracting its information
		rm(source_dir->parent, file_name);

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

				if (child->info->type == FOLDER_NODE) {
					next_dir = child->info;
				} else {
					// The destination is a file -> replacing its content
					if (((FileContent *)child->info->content)->text)
						free(((FileContent *)child->info->content)->text);
					((FileContent *)child->info->content)->text = strdup(file_content);

					done_mv = 1;
					break;
				}
			}
			list = ((FolderContent *)next_dir->content)->children;
			next_directory = strtok(NULL, "/");
		}

		if (!done_mv) {
			// Create new file identical to the source:
			touch(next_dir, file_name, file_content);
		}

		free(aux_dest);
	}
}
