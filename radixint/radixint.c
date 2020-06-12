//Matias Lago, HW4, 2019
//Assume nonnegative 32-bit wide integer input
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int val;
	struct node *next;	
} node_t;

void print_list(node_t *head);
void bucket_sort(node_t **head, int len);
void add(node_t *head, int val);

int main(int argc, char **argv) {
	
	//populate list of values	
	int n, count = 0;
	node_t *values = malloc(sizeof(node_t));
	values->val = -1;
	while(scanf("%d", &n) != EOF) {
		add(values, n);
		count++;
	}	
	
	//sort the list with bucket sort	
	bucket_sort(&values, count);	
	
	//print the list	
	print_list(values);
	
	//return memory to heap
	free(values);

	return 0;
}

//add a specific value to end/beginning of list
void add(node_t *head, int val) {
	if(head->val == -1) {
		head->val = val;
		head->next = NULL;
	}
	else {
		node_t *curr = head;
		while(curr->next != NULL)
			curr = curr->next;
		curr->next = malloc(sizeof(node_t));
		curr->next->val = val;
		curr->next->next = NULL;
	}
}

//sort a linked list via radix sort (base = 16)
void bucket_sort(node_t **head, int len) {	
	//visit everything in list, put in appropriate bucket
	for(int pass=0; pass < 8; pass++) {
		//each nibble/letter (4 bits) can be retrieved with
		//(n >> (4*pass)) & 0xF where p = 0, 1, 2 is pass number
		node_t *buckets = calloc(16, sizeof(node_t));
		for(int i=0; i < 16; i++)
			buckets[i].val = -1;
		
		node_t *curr = *head;
		while(curr != NULL) {
			int to_bucket = ((curr->val) >> (4*pass)) & 0xF;
			add(&buckets[to_bucket], curr->val);
			curr = curr->next;
		}

		//all are in their appropriate buckets, now put each bucket back to head in order
		node_t *new_head = malloc(sizeof(node_t));
		new_head->val = -1;
		for(int i=0; i< 16; i++) {
			node_t *curr_bucket = &buckets[i];
			while(curr_bucket != NULL) {
				if(curr_bucket->val == -1)
					break;
				add(new_head, curr_bucket->val);
				curr_bucket = curr_bucket->next;
			}
		}
		
		//"update" head to be up to speed with latest pass, "reset" buckets
		*head = new_head;
		free(buckets);
	}
}

//print list
void print_list(node_t *head) {
	node_t *curr = head;
	while(curr != NULL) {
		if(curr->val >= 0) //only interpret nonnegative values as valid, like spec said
			printf("%d\n", curr->val);
		curr = curr->next;
	}
}
