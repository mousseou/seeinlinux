#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

#define LEFT 0
#define UP   1
#define RIGHT 2
#define DOWN 3

struct node {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;	/* location (row and colum) of blank tile 0 */
	struct node *next;
	struct node *parent;			/* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];
struct node *start,*goal;
struct node *open = NULL, *closed = NULL;
struct node *succ_nodes[4];

void print_a_node(struct node *pnode) {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) 
			printf("%2d ", pnode->tiles[i][j]);
		printf("\n");
	}
	printf("\n");
}

struct node *initialize(char **argv){
	int i,j,k,index, tile;
	struct node *pnode;

	pnode=(struct node *) malloc(sizeof(struct node));
	index = 1;
	for (j=0;j<N;j++)
		for (k=0;k<N;k++) {
			tile=atoi(argv[index++]);
			pnode->tiles[j][k]=tile;
			if(tile==0) {
				pnode->zero_row=j;
				pnode->zero_column=k;
			}
		}
	pnode->f=0;
	pnode->g=0;
	pnode->h=0;
	pnode->next=NULL;
	pnode->parent=NULL;
	start=pnode;
	//printf("initial state\n");
	//print_a_node(start);

	pnode=(struct node *) malloc(sizeof(struct node));
	goal_rows[0]=3;
	goal_columns[0]=3;

	for(index=1; index<NxN; index++){
		j=(index-1)/N;
		k=(index-1)%N;
		goal_rows[index]=j;
		goal_columns[index]=k;
		pnode->tiles[j][k]=index;
	}
	pnode->tiles[N-1][N-1]=0;	      /* empty tile=0 */
	pnode->f=0;
	pnode->g=0;
	pnode->h=0;
	pnode->next=NULL;
	goal=pnode; 
	//printf("goal state\n");
	//print_a_node(goal);

	return start;
}

/* swap two tiles in a node */
void swap(int row1,int column1,int row2,int column2, struct node * pnode){
	int tile = pnode->tiles[row1][column1];
	pnode->tiles[row1][column1]=pnode->tiles[row2][column2];
	pnode->tiles[row2][column2]=tile;
}

/* 0 goes down by a row */
void move_down(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row+1, pnode->zero_column, pnode); 
	pnode->zero_row++;
}

/* 0 goes right by a column */
void move_right(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column+1, pnode); 
	pnode->zero_column++;
}

/* 0 goes up by a row */
void move_up(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row-1, pnode->zero_column, pnode); 
	pnode->zero_row--;
}

/* 0 goes left by a column */
void move_left(struct node * pnode){
	swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column-1, pnode); 
	pnode->zero_column--;
}

int nodes_same(struct node *a,struct node *b) {
	int flg=FALSE;
	if (memcmp(a->tiles, b->tiles, sizeof(int)*NxN) == 0)
		flg=TRUE;
	return flg;
}

//move to a given direction (if possible, else pnode remains unchanged)
void move_dir(struct node * pnode, int direction) {
    switch(direction) {
        case LEFT: 
            if(pnode->zero_column != 0)
                move_left(pnode); 
            break;
        case RIGHT: 
            if(pnode->zero_column != N-1)
                move_right(pnode); 
            break;
        case DOWN: 
            if(pnode->zero_row != N-1)
                move_down(pnode); 
            break;
        case UP: 
            if(pnode->zero_row != 0)
                move_up(pnode); 
            break;
    }
}

//get value of h for computing f in A* for each node
int calculateHeuristic(struct node *pnode) {
    int misplaced = 0;
    for(int i=0; i < N; i++) {
        for(int j=0; j < N; j++) {
            if(pnode->tiles[i][j] != 4*i + j + 1)
                misplaced++;
        }
    }
    return misplaced;
}

/* expand a node, get its children nodes, and organize the children nodes using
 * array succ_nodes.
 */
void expand(struct node *selected) {
    /* List of all possible successor states:
        You can either move up, down, left or right (That's why there's 4 succ_nodes) 
        Let 0->left, 1->up, 2->right, 3->down
        Populate succ_nodes

        Incorporate A* algorithm in, calculate f, g, h
    */
    for(int i=0; i < 4; i++) 
    {
        succ_nodes[i] = (struct node *)(malloc(sizeof(struct node)));
        memcpy(succ_nodes[i], selected, sizeof(struct node));
        move_dir(succ_nodes[i], i);
        succ_nodes[i]->g++;
        succ_nodes[i]->h = calculateHeuristic(succ_nodes[i]);
        succ_nodes[i]->f = succ_nodes[i]->g + succ_nodes[i]->h;
    }
}

void push_index(int index, struct node *val) { 
    if(open) {
        struct node *curr = open;
        int i=0;
        while(curr != NULL) {
            if(index == 0) {
                val->next = curr;
                open = val;
                break;
            }
            else if(i == index-1) {
                if(curr->next)
                    val->next = curr->next;
                curr->next = val;
                break; //insert, found the index
            }
            
            curr = curr->next;
            i++;
        }

    }
    else 
        open = val;

    //printf("DBG: A node was successfully inserted\n");
}

void push(struct node *val) {
    if(open) {
        struct node *curr = open;
        while(curr->next != NULL) {

            curr = curr->next;
        }
        val->next = NULL;
        curr->next = val;
    }
}

/*
void dbg_print_all_nodes(struct node *parent) {
    struct node *curr = parent;
    printf("Printing the each node in parent (with their f value)\n");
    while(curr != NULL) {
        printf("---- Value %d ----\n", curr->f);
        print_a_node(curr);
        printf("------------------\n");
        curr = curr->next;
    }
}*/

/* merge unrepeated nodes into open list after filtering */
void merge_to_open(struct node *parent) { 

    for(int i=0 ; i < 4; i++) {
        if(succ_nodes[i]) {
            succ_nodes[i]->parent = parent;

            //compare succ_nodes[i]->f to f values for elements already in open
            int index = 0;
            if(open) {
                struct node *curr = open;
                
                while(curr != NULL) {
                    if(succ_nodes[i]->f > curr->f)
                        index++;
                    else // succ_node[i] is smaller than open[index], so it must go before it at this spot
                        break;
                    
                    curr = curr->next;
                }
                if(curr == NULL) //if null, index may/not be 0, indecisive. use -1 as failsafe
                    index = -1;
            }
            if(index == -1)
                push(succ_nodes[i]);
            else
                push_index(index, succ_nodes[i]);
        }
            
    }
}


/* Filtering. Some nodes in succ_nodes may already be included in either open 
 * or closed list. Remove them. It is important to reduce execution time.
 * This function checks the (i)th node in succ_nodes array. You must call this
 & function in a loop to check all the nodes in succ_nodes.
 */ 
void filter(int i, struct node *pnode_list) {
    //closed gets called as null
    int n = 0;
    while((pnode_list != NULL) && (succ_nodes[i] != NULL)) {
        n++;
        //if node on succ[i] is same as on pnode_list (ie it is there), remove it
        if(nodes_same(succ_nodes[i], pnode_list)) {
            succ_nodes[i] = NULL;
            break; //break out, we've already ruled it out
        }
        pnode_list = pnode_list->next; //may be wrong to alter list here
    }
    
}


/* ---------------------------------------------------------------------------------------- */

int main(int argc,char **argv) {
	int iter,cnt;
	struct node *copen, *cp, *solution_path;
	int ret, i, pathlen=0, index[N-1];

	solution_path=NULL;
	start=initialize(argv);	/* init initial and goal states */
	open=start;

	iter=0; 
	while (open!=NULL) {	/* Termination cond with a solution is tested in expand. */
		copen=open; /* N = copen, pop this from open */
		open=open->next;  /* get the first node from open to expand */
		if(nodes_same(copen,goal)){ /* goal is found / N is goal state*/
			do{ /* trace back and add the nodes on the path to a list */
				copen->next=solution_path;
				solution_path=copen;
				copen=copen->parent;
				pathlen++;
			} while(copen!=NULL);
			printf("Path (length=%d):\n", pathlen); 
			copen=solution_path;
			/* print out the nodes on the list */
            while(copen != NULL) {
                print_a_node(copen);
                copen = copen->next;
            }
			break;
		}
		expand(copen);       /* Find new successors to n*/
		for(i=0;i<4;i++){
			filter(i,open);
			filter(i,closed);
		}
		merge_to_open(copen); /* New open list - when you put something in open, make sure it has its parent!*/
        if(closed)
		    copen->next=closed;
		closed=copen;		/* New closed */
		/* print out something so that you know your 
		 * program is still making progress 
		 */
		iter++;
		if(iter %1000 == 0)
			printf("iter %d\n", iter);
	}
	return 0;
} /* end of main */
