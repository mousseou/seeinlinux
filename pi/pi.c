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

// ./program number_processes number_digits

double mypi = 0;
double PI25DT = 3.141592653589793238462643;

int main(int argc, char **argv) {

    if(argc != 3) {
        printf("Usage: %s number_of_processes number_of_digits\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pfds[2], retval;
    int N = atoi(argv[2]);
    int np = atoi(argv[1]);

    //Restriction spoken about below is placed here.
    if(np > 2*N) {
                fprintf(stderr, "Number of processes desired (%d) exceeds maximum value of (%d) for approximation to %d digits\n",
                            np, 2*N, N);
                exit(EXIT_FAILURE);
    }
        
    int ids[np];
    double *sum = malloc(sizeof(double) * N);
    pipe(pfds);
    for(int ii=0; ii < np; ii++) {
        if((ids[ii] = fork()) == 0) {
            //work for a specific child with id "ii"
            double x = 0;
            double h = 1.0 / (double)N;
            *(sum+ii) = 0;
            double lb = (double)N/np*ii+1;
            double ub = (double)N/np*(ii+1);

            //don't use i as an integer -> have it be a double to avoid "odd" problems (when N/np has a remainder)
            //NOTE: algorithm works with some np > N, specifically up to the value of np = 2N. 
            //That is, malloc fails if np > 2N. The critical value would result in N/np = N/2N = .5. 
            //Any larger denominator and the result be smaller than .5 -> this leads to poorly evaluated bounds.
            //Have tried to increase the size of malloc'd memory to try to see if this would be avoided -> it doesn't.
            //Thus, since spec said nothing about how to handle this, I've chosen to inform the user of this restriction.   
            //Restriction is above, before children are created.

            for(int i=lb; i <= ub; i++) {
                x = h * ((double)i - 0.5);
                *(sum+ii) += 4.0 / (1.0 + x*x);
            }
            //printf("(%d) I am child id %d and I computed sum %f\n", ii, getpid(), *(sum+ii));
            //printf("My interval was [%f, %f]\n", lb, ub);
            write(pfds[1], sum+ii, sizeof(double));
            exit(EXIT_SUCCESS); // need to exit here to avoid parent creating extra unnecessary process
        }
    }

    //wait until they're all done computing PI
    //there's np active processes - wait for any to finish and decrement the number
    //once it reaches 0, then no children are running and multiplication is done
    while(np > 0) {
        int status;
        int pid = wait(&status);
        --np;
    }


    close(pfds[1]); //close when parent wants to read
    retval = read(pfds[0], sum, sizeof(double) * atoi(argv[1]));

    for(int i=0; i < atoi(argv[1]); i++) {
        mypi += 1.0 /(double) N * sum[i];
    }
        

    //computation is done here, print the approximation of PI
    printf("pi is approximately %.16f, Error is %.16f\n", mypi, fabs(mypi - PI25DT));
    free(sum);
    exit(EXIT_SUCCESS);
}