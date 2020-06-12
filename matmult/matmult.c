#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <assert.h>
#include <sys/wait.h>


float *smap; /* source map, smap (read matrix from here */
float *tmap; /* target map, tmap (write matrix to here */

void print_matrix(float *m, int N) {
    for(int i=0; i < N; i++) {
        for(int j=0; j < N; j++)
            printf("%.2f ", *(m+i+j*N));
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv) {

    if(argc != 4) {
        printf("Correct usage: %s number_of_processes file_containing_matrix file_saving_product_matrix\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //open files
    int sfd = open(argv[2], O_RDONLY);
    if(sfd == -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    int tfd = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if(tfd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    //get info from file here (size - both will share the same size)
    struct stat statBuf;
    int result = stat(argv[2], &statBuf);
    if(result == -1) {
        fprintf(stderr, "Cannot stat %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    ssize_t nf = statBuf.st_size/sizeof(float);
    ssize_t N = (int)sqrt(nf);
    if(N != sqrt(nf)) {
        fprintf(stderr, "The matrix provided (size=%ld) is not square. \n", N);
        exit(EXIT_FAILURE);
    }

    //Generate identity matrix here
    float *identity = malloc(sizeof(float) * N * N);
    for(int i=0; i < N; i++)
        *(identity+i+i*N) = 1;
    //print_matrix(identity, N);

    result = lseek(tfd, nf * sizeof(float)-1, SEEK_SET);
    if (result == -1) {
        close(tfd);
        perror("Error calling lseek() to 'stretch' the target file");
        exit(EXIT_FAILURE);
    }

    result = write(tfd, "", 1);
    if (result != 1) {
        close(tfd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    //map the files
    smap = mmap(0, nf * sizeof(float), PROT_READ, MAP_SHARED, sfd, 0);
    if(smap == MAP_FAILED) {
        close(sfd);
        perror("Error mapping the source file");
        exit(EXIT_FAILURE);
    }

    tmap = mmap(0, nf * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, tfd, 0);
    if (tmap == MAP_FAILED) {
        close(tfd);
        perror("Error mapping the target file");
        exit(EXIT_FAILURE);
    }

    //print_matrix(smap, N);


    // Use files here
    // Create child processes, assign each one to a specific "chunk" of the target matrix to write
    int np = atoi(argv[1]);
    int ids[np];
    for(int ii=0; ii < np; ii++) {
        if((ids[ii] = fork()) == 0) {
            //work for a specific child with id "ii"
            int adjusted = (int)ceil((double)N/np); //need to adjust the N/np calculation for when it has a "remainder" -> ROUND UP to account for rows of last column
            //printf("DBG: %d %d %d\n", N, np, adjusted);
            for(int i=0; i < N; i++) {
                for(int j=adjusted*ii; j < adjusted*(ii+1); j++) {
                    for(int k=0; k < N; k++)
                        *(tmap+i+j*N) = *(tmap+i+j*N) + *(smap+i+k*N) * *(identity+k+j*N);
                }
            }
            exit(EXIT_SUCCESS); // need to exit here to avoid parent creating extra unnecessary process
        }
    }

    //Make parent wait for everyone to be done
    //there's np active processes - wait for any to finish and decrement the number
    //once it reaches 0, then no children are running and multiplication is done
    while(np > 0) {
        int status;
        int pid = wait(&status);
        --np;
    }

    //print_matrix(tmap, N);

    
    //Unmap & close
    if(munmap(smap, nf * sizeof(float)) == -1)
        perror("Error unmapping the source file");
    if(munmap(tmap, nf * sizeof(float)) == -1)
        perror("Error unmapping the target file");
    close(sfd);
    close(tfd);
    free(identity);
    exit(EXIT_SUCCESS);
}