/* Matias Lago, 2019, Minor #5 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>


void bucket_sort(float *start, int len);
void print_float(const float f);
void print_map(float *map);

typedef struct node {
	float val;
	struct node *next;	
} node_t;

long long NUM_FLOATS = 0;
size_t FILE_SIZE = 0;

//tail, it will point at the last element at all times making pushing to end of list faster
node_t *tails[2] = {NULL, NULL};

int main(int argc, char **argv) {

    if(argc != 2) {
        perror("Usage: %s <path_to_file>");
        exit(EXIT_FAILURE);
    }

    int fd;
    float *map;

    fd = open(argv[1], O_RDWR);
    if(fd == -1) {
        perror("Error opening file for reading/writing");
        exit(EXIT_FAILURE);
    }

    //with the file descriptor, use fstat to get filesize and number of floats
    struct stat *buf;
    fstat(fd, buf);
    FILE_SIZE = buf->st_size;
    NUM_FLOATS = FILE_SIZE / sizeof(float);

    int seek_result = lseek(fd, FILE_SIZE-1, SEEK_SET);
    if(seek_result == -1) {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    map = mmap(0, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
     

    /* printf("BEFORE SORT:\n");
    print_map(map); */

    //modify map(memory) directly while sorting!
    bucket_sort(map, NUM_FLOATS);
    
    /* printf("AFTER SORT:\n");
    print_map(map); */

    //Our mapped memory now contains sorted values! Unmap and close
    if(munmap(map, FILE_SIZE) == -1)
        perror("Error unmapping the file");
    
    close(fd);
    
    return 0;
}

/* Sorting/Bucketsort related functions
------------------------------------------------------------------------------
*/

//add a specific value to end/beginning of list
void add(node_t *head, int val, int bucket) {
    //bucket is valid
	if(head->val == INFINITY) {
		head->val = val;
		head->next = NULL;
        tails[bucket] = head;
	}
    
	else { 
		node_t *curr = tails[bucket];
        curr->next = malloc(sizeof(node_t)); 
        
		curr->next->val = val;
		curr->next->next = NULL;
        tails[bucket] = curr->next;
	}
}

void swap(float *a, float *b) {
    float temp = *b;
    *b = *a;
    *a = temp;
}

void bucket_sort(float *start, int len) {
    // We'll be sorting individual bits at a time.
    for(int pass=0; pass < 32; pass++) {
        node_t *buckets = calloc(2, sizeof(node_t));
        for(int i=0; i < 2; i++) //INFINITY is used as a sentinel value for buckets
            buckets[i].val = INFINITY;


        for(long int i=0; i < NUM_FLOATS; i++) {
            unsigned int *pi = (unsigned int *)(start + i);
            int to_bucket = ((*pi) >> pass) & 0x1;
            if(pass == 5 && i > 34300000 && i % 10000 == 0) {
                printf("Currently at: %li\n",i);
            }
            add(&buckets[to_bucket], *(start+i), to_bucket);
        }
        
        //all numbers are sorted for a given pass (from right -> left)
        //dump values into head
        int count = 0;
		for(int i=0; i< 2; i++) {
			node_t *curr_bucket = &buckets[i];
			while(curr_bucket != NULL) {
				if(curr_bucket->val == INFINITY)
					break;
                //copy this bucket's content one by one into our map
				memcpy(start + count, &curr_bucket->val, sizeof(float));
				curr_bucket = curr_bucket->next;
                count++;
			}
		}

		free(buckets);
    }
    
    //At this point, we have [ordered nonnegative][unordered negative]
    //Find first negative, and swap negatives in place on map
    int first_negative = 0;
    for(long int i=0; i < NUM_FLOATS; i++) {
        if( *(start + i) < 0) {
            first_negative = i;
            break;
        }
    }

    for(long int i=first_negative, j=NUM_FLOATS-1; i < j; i++, j--) {
        swap(start + i, start + j);
    }

    //first copy positives over to a different spot
    float *positives = malloc(first_negative * sizeof(float));
    memcpy(positives, start, first_negative * sizeof(float));
    //then, "move" the negatives within the map to be at the beginning
    memmove(start, start+first_negative, (NUM_FLOATS - first_negative)*sizeof(float));
    //finally, "move" the positives from the different spot to right after the negatives
    memcpy(start+NUM_FLOATS-first_negative, positives, first_negative * sizeof(float));

    free(positives);
}

/* Debug/Helper Functions 
------------------------------------------------------------------------------
*/
void print_float(const float f) { /* assume LE - LSB comes first */
    unsigned int *pi = (unsigned int *)&f;
    char bits[33];
    bits[32] = '\0';

    for(int i = 0; i < 32; i++)
        bits[31-i] = (((*pi) >> i) & 0x1) ? '1' : '0';

    printf("%f -> ", f);
    for(int i = 0; i < 32; i++) {
        printf("%c", bits[i]);
        if((i+1) % 4 == 0)
            printf(" ");
    }
}

void print_map(float *map) {
    for(long int i=0; i < NUM_FLOATS; i++) {
        printf("%ld: ", i);
        print_float(map[i]);
        printf("\n");
    }
}