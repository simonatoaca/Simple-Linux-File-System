#ifndef _TREE_H_
#define _TREE_H_

#define LINE_MAX_LEN 1000
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;
typedef struct ListNode ListNode;
typedef struct List List;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    List* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

struct ListNode {
    TreeNode* info;
    ListNode* next;
};

struct List {
    ListNode* head;
};

/*
	@brief shows elements in a directory or shows the content of the
        file specified by <arg>
    @param currentNode - the current directory
    @param arg - name of a file or NO_ARG
*/
void ls(TreeNode* currentNode, char* arg);

/*
	@brief prints the path to the current directory, starting
        from the root
    @param treeNode - the current directory
*/
void pwd(TreeNode* treeNode);

/*
	@brief moves user to the directory specified by <path>
    @param currentNode - the current directory
    @param path - path to be followed
    @return the destination node
*/
TreeNode* cd(TreeNode* currentNode, char* path);

/*
	@brief prints the file tree
    @param currentNode - the current directory
    @param arg - path relative to currentNode from where
        the tree starts
*/
void tree(TreeNode* currentNode, char* arg);

/*
	@brief creates a new (empty) folder in the current directory
    @param currentNode - the current directory
    @param folderName - the name of the new folder
*/
void mkdir(TreeNode* currentNode, char* folderName);

/*
	@brief removes a file from a directory
    @param currentNode - the current directory
    @param fileName - the name of the file to be removed
*/
void rm(TreeNode* currentNode, char* fileName);

/*
	@brief removes a directory (only if empty) from the current dir
    @param currentNode - the current directory
    @param folderName - the name of the folder to be removed
*/
void rmdir(TreeNode* currentNode, char* folderName);

/*
	@brief removes a file/folder(regardless if empty or not)
        from the current directory
    @param currentNode - the current directory
    @param resourceName - the name of the file/folder
*/
void rmrec(TreeNode* currentNode, char* resourceName);

/*
	@brief creates a new file in the current directory
    @param currentNode - the current directory
    @param fileName - the name of the new file
    @param fileContent - the text inside the file (if specified)
*/
void touch(TreeNode* currentNode, char* fileName, char* fileContent);

/*
	@brief copies a file/empty directory from source to destination
    @param source - the source directory + name of file/folder
    @param destination - the destination directory + name of file/folder
*/
void cp(TreeNode* currentNode, char* source, char* destination);

/*
	@brief moves a file/directory from source to destination
    @param source - the source directory + name of file/folder
    @param destination - the destination directory
*/
void mv(TreeNode* currentNode, char* source, char* destination);

/*
	@brief creates the file tree, initializing the root
    @param rootFolderName - the name of the root directory
    @return a pointer to the file tree
*/
FileTree *createFileTree(char* rootFolderName);

/*
	@brief frees the file tree
    @param the file tree to be freed
*/
void freeTree(FileTree *fileTree);
#endif
