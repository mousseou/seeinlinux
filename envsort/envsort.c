#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bubbleSort(char **arr, int len) {
    for(int pass=0; pass < len - 1; pass++) {
        for(int j=0; j < len - 1; j++) {
            if(strcmp(*(arr+j), *(arr+j+1)) > 0) {
                char *temp = *(arr+j);
                *(arr+j) = *(arr+j+1);
                *(arr+j+1) = temp;
            }
        }
    }
}

int main(int argc, char **argv, char **envp) {

    int len = 0; /* Get the number of env vars */
    while(envp[len])
        len++;
    
    bubbleSort(envp, len); /* Sort in place */

    len = 0;
    while(envp[len]) { /* Print the sorted results */
        char *name = strtok(envp[len], "=");
        printf("%s=%s\n", name, strtok(NULL, "\n"));
        len++;
    }

    exit(EXIT_SUCCESS);
}