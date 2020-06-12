#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <assert.h>

float *map = NULL;
double sum = 0;
int NUM_THREADS = 0;
int NUM_FLOATS = 0;
int thread_id = 0;

void *worker(void *arg) {
    int tid = *(int *)arg;
    /* Evaluate the number of equal chunks, and chunksize.
        If F % T == 0, then we can fit everything in "T" equal chunks, and the size of them is F/T respectively
        If F % T != 0, then we can fit everything in "T" equal chunks, and the size of them is (F/T)+1 respectively

    Note: this approach overestimates the "work" for threads when F % T != 0, but does so by a negligible amount */
    int echunks = (NUM_FLOATS % NUM_THREADS) ? (NUM_FLOATS % NUM_THREADS) : NUM_THREADS;
    int echunksize = (NUM_FLOATS % NUM_THREADS) ? (NUM_FLOATS/NUM_THREADS) +1 : NUM_FLOATS/NUM_THREADS;
    for(int i = echunksize*tid; i < (echunksize*tid)+echunksize; i++) {
        //printf("I am thread id %d and I evaluated position %d\n", tid, i);
        if(map + i != NULL)
            sum += map[i];
    }
}

int main(int argc, char **argv) {

    if(argc != 3) {
        printf("Correct usage: %s number_of_threads file_to_evaluate\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //open file
    int fd = open(argv[2], O_RDONLY);
    if(fd == -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    //get info from file here (size)
    struct stat statBuf;
    int result = stat(argv[2], &statBuf);
    if(result == -1) {
        fprintf(stderr, "Cannot stat %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    ssize_t FILESIZE = statBuf.st_size;
    NUM_FLOATS = FILESIZE / sizeof(float);

    //map the file
    map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
    if(map == MAP_FAILED) {
        close(fd);
        perror("Error mapping the file");
        exit(EXIT_FAILURE);
    }

    //use the file's contents here as an array (ie our "main" program)
    
    NUM_THREADS = atoi(argv[1]);

    pthread_t threads[NUM_THREADS];
    int myid[NUM_THREADS];
    int i, tresult;
    

    for(i = 0; i < NUM_THREADS; i++) {
        myid[i] = i;
        tresult = pthread_create(&threads[i], NULL, worker, &myid[i]);
        assert(result == 0);
    }

    
    for(i = 0; i < NUM_THREADS; i++) {
        tresult = pthread_join(threads[i], NULL);
        assert(result == 0);
    }

    printf("Final value (sum): %f\n", sum);
    

    //unmap & close
    if(munmap(map, FILESIZE) == -1) {
        perror("Error unmapping the file");
    }
    close(fd);
    exit(EXIT_SUCCESS);
}