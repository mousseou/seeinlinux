#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
extern int errno;

void processDirectory(char *dirName);
void handleFile(char *fileName);

typedef struct node {
    ssize_t val;
    char *path;
    struct node *next;
} node_t;

node_t *head = NULL;

void print_list(node_t *head) {
    node_t *curr = head;

    while(curr != NULL) {
        printf("%ld\t%s\n", curr->val, curr->path);
        curr = curr->next;
    }
}

void add_to_start(node_t **head, ssize_t fsize, char *realpath) {
    node_t *node = malloc(sizeof(node_t));
    node->val = fsize;
    node->path = malloc(514);
    memcpy(node->path, realpath, 514);
    node->next = NULL;
    *head = node;
}

void add(node_t **head, ssize_t fsize, char *realpath) {
    if(*head == NULL) /* list is empty, add at beginning */
        add_to_start(head, fsize, realpath);
    else if((*head)->val > fsize) { /* if first val > fsize, "update" head */
        node_t *node = malloc(sizeof(node_t));
        node->val = fsize;
        node->path = malloc(514);
        memcpy(node->path, realpath, 514);
        node->next = *head;
        *head = node;
    }
    else {
        node_t *curr = *head;
        while(curr->next != NULL) {
            if(curr->next->val > fsize)
                break;
            curr = curr->next;
        }
            
        if(curr->next == NULL) { /*Reached the end of list and not found, so insert here */
            curr->next = malloc(sizeof(node_t));
            curr->next->val = fsize;
            curr->next->path = malloc(514);
            memcpy(curr->next->path, realpath, 514);
            curr->next->next = NULL;
        } else { /* the next value is bigger than the one to be inserted, so we insert here */ 
            node_t *node = malloc(sizeof(node_t));
            node->val = fsize;
            node->path = malloc(514);
            memcpy(node->path, realpath, 514);
            node->next = curr->next;
            curr->next = node;
        }
    }
}

void handleFile(char *fileName) { /* Process a file/directory */
    //processDirectory() already ensures that we receive only a regular file here
    //fileName must hold "complete" path to file, else stat will fail
    struct stat statBuf; 
	mode_t mode; 
	int result; 

	result = lstat(fileName, &statBuf); /* Obtain file status */ 
	if ( result == -1 ) { /* Status was not available */ 
		fprintf( stderr, "Cannot stat %s \n", fileName ); 
		return; 
	} 
    
    /* Add to list based on size, have it sorted already so inserting is "easier" */
    //printf("%ld\t%s\n", statBuf.st_size, fileName);
    
    add(&head, statBuf.st_size, fileName);
}

void processDirectory(char *dirName) { /* Process files in a directory */
    int charsRead;
    DIR *dir;
    struct dirent *dirEntry;

    dir = opendir(dirName);
    while(1) {
        if(dir == NULL) break; 
        dirEntry = readdir(dir);
        if(dirEntry == NULL) break; /* EOF */
        /* Avoid looking at ./ and ../ directories, else stuck on endless loop */
        /* evaluate dirName + "/" + dirEntry->d_name for complete path 
           from dirent, d_name can be up to 256 characters. Thus, we need 256 + "/" + 256 possibly.
            Hence, the message will need to be at most 514 (to hold / and NULL too)*/
        char path[514];
        int i=0, j =0;
        while(dirName && dirName[i] != '\0') {
            path[i] = dirName[i];
            i++;
        }
        path[i++] = '/';
        while(dirEntry->d_name && dirEntry->d_name[j] != '\0') {
            path[i] = dirEntry->d_name[j];
            i++, j++;
        }
        path[i] = '\0';
            
        //printf("RealPath: %s\n", path);
        if(dirEntry->d_type == DT_DIR && strcmp(dirEntry->d_name, ".") != 0 && strcmp( dirEntry->d_name, "..") != 0)
            processDirectory(path); /* Recursively deal with directories */
        else if(dirEntry->d_type == DT_REG)
            handleFile(path);
    }
}


int main(int argc, char **argv) {

    if(argc != 2) {
        printf("Usage: %s <path_to_evaluate>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //populate list after looking recursively through directory, sort upon insertion
    processDirectory(argv[1]);

    //print
    print_list(head);
    
    exit(EXIT_SUCCESS);
}