**Toacă Alexandra Simona & Petrache Gabriela Andreea,**
**312CA**

## Simple Linux File System - The Third Data Structures Homework

### Description:

* The purpose of this project is to replicate the Linux File System, implemented in C.
* The system is composed of directories and files.
* We have implemented 11 possible commands for the system.
* In order to create a file, the user can type the command "touch" followed by the name of the file, and its content, if needed. The file is created in the current directory.
* For directories, the user can use the command "mkdir" followed by the name of the directory. The directory is also created in the current directory.
* The user can change the current directory by using the command "cd" followed by the path to change to.
* For listing the content of a directory or a file, we have implemented the command "ls", which lists the content of the current directory if it is not followed by any other arguments, otherwise, it prints the content of the argument file.
* The "pwd" command will print the path to the current directory.
* "tree" will print the directory tree of either current directory, or the path mentioned in the argument.
* We have three functions that can be used for removing files and directories. The first one is "rm" which removes the file specified in the argument. The second one is "rmdir" which removes the directory mentioned in the argument, as long as it is empty. The third one is "rmrec" which removes the directory and all its content.
* The "cp" command will copy the argument to the mentioned path. For copying a file to another file, the content of the destination file will be overwritten.
* The "mv" command will move the argument to the mentioned path. For not empty directories, the content of the directory will be moved to the destination directory.
* For this implementation, we used several data structures to store the information, which can be found in the "tree.h" file. 

### Comments:

* This project was a great opportunity to practice working as a team.
* It was a great experience to work together and to listen to each other's ideas and decide on the best way to implement the project.
* We managed to expand our knowledge of C and its data structures, by implementing this simple file system.
* One issue we had was having to duplicate our code in certain sections (such as cp, mv), in order to cover all the possible error messages for each command.
