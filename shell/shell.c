#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
extern int errno;

#define MAX_INPUT 1000

int main(int argc, char **argv, char **envp) {

    char *input = malloc(MAX_INPUT);
    char *command_name;
    int result;

    while(1) {
        printf("$$>"); //show prompt and read in the line
        ssize_t n, len = getline(&input, &n, stdin);
        input[len-1] = '\0'; //remove the newline at the end 

        command_name = strtok(input," "); //get the command name (first token from space-delimiter)
        if(command_name == NULL) // an empty command (ie only line/space from token, just ignore)
            continue; 

        if(strncmp(command_name, "exit", 4) == 0) //exit if user enters command "exit"
            break;

        /* Populate arguments array to pass to exec: 
            First put the name of the program/path to execute
            then follow by all the arguments
            and end by a NULL pointer to follow spec */
        
        char **args = calloc(MAX_INPUT, MAX_INPUT); //conservative estimate for arguments 
        args[0] = command_name;
        int written = 1;

        char *tmp = strtok(NULL, " ");
        while(tmp != NULL) {
            //Handle "simple" quoted arguments (those that are one arg bounded by quotes, w/o spaces)
            if(tmp[0] == '\"') {
                tmp[strlen(tmp)-1] = '\0'; // Simply "trim" string accordingly
                tmp+=1;
            }
            args[written++] = tmp;
            tmp = strtok(NULL, " ");
        }
        args[written] = NULL;
        
        /* Create a child process, call exec appropriately for specific command, wait for it to finish */
        int pid, status;
        if((pid = fork()) == 0) {
            //work for specific child
            //ensure args is good
            execvp(command_name, args);

            //THIS PART WILL ONLY EXECUTE IF EXECVP FAILED
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        
        pid = wait(&status);
        free(args);
    }

    free(input);
    exit(EXIT_SUCCESS);
}

