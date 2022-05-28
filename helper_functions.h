#ifndef _HELPER_FUNCTIONS_H_
#define _HELPER_FUNCTIONS_H_
#include "tree.h"
#include "list.h"

/*
	@brief recursive helper function for freeing the tree
    @param tree_node - the current node
*/
void __free_helper(TreeNode *tree_node);

/*
	@brief recursive helper function for printing the tree hierarchy
    @param tree_node - the starting directory
	@param level - the level of the current dir relative to the starting dir
	@param dirs - number of directories passed so far
	@param files - number of files passed so far
*/
void __tree_helper(TreeNode *tree_node, int *level, int *dirs, int *files);

/*
	@brief follows the path to the source file and gets its name and content
    @param currentNode - the current directory
	@param source_node - if != NULL, then the file is in the current dir
	@param source - the path to the source file
	@param file_name - the source file name
	@param file_content - the source file content (can be NULL)
	@return 1 if file exists, 0 otherwise
*/
int get_file_info(TreeNode *currentNode, ListNode *source_node, char *source,
				  char **file_name, char **file_content);

/*
	@brief follows the path to the source folder and gets its name and content
    @param currentNode - the current directory
	@param source_node - if != NULL, then the folder is in the current dir
	@param source - the path to the source folder
	@param file_name - the source folder name
	@param file_content - the source folder content - a list of files and dirs
	@return 1 if folder exists, 0 otherwise
*/
int get_folder_info(TreeNode *currentNode, ListNode *source_node, char *source,
				  char **folder_name, List **folder_content);

/*
	@brief 
    @param currentNode - the current directory
	@param source - the path to the source folder
	@param destination - the path to the destination folder
*/
void mv_rec(TreeNode *currentNode, char *source, char *destination);

/*
	@brief frees a ListNode according to its type (FILE or FOLDER)
    @param curr - the list node to be freed
*/
void custom_free(ListNode *curr);
#endif


