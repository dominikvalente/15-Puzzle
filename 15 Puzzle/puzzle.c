#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct node {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;
	struct node *next;
	struct node *parent;
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
	printf("initial state\n");
	print_a_node(start);

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
	printf("goal state\n");
	print_a_node(goal);

	return start;
}

/* merge unrepeated nodes into open list after filtering */
void merge_to_open() {

    //if successor node = NULL, skip iteration
    for(int i = 0; i < N; i++){
        if(succ_nodes[i] == NULL){
            continue;
        }

        //Create a new node to insert, as succ_nodes will soon be cleared for next iteration
        struct node *newNode = (struct node *) malloc(sizeof(struct node));

        memcpy(newNode->tiles, succ_nodes[i]->tiles, NxN*sizeof(int));
        newNode->f = succ_nodes[i]->f;
        newNode->g = succ_nodes[i]->g;
        newNode->h = succ_nodes[i]->h;
        newNode->zero_row = succ_nodes[i]->zero_row;
        newNode->zero_column = succ_nodes[i]->zero_column;
        newNode->parent = succ_nodes[i]->parent;

        if (open == NULL){
            open = newNode;
            continue;
        }
        struct node *t = open;

        int node_inserted = FALSE;
        while(t != NULL && t->next != NULL){
            //priority
            if(newNode->f < t->next->f){
                newNode->next = t->next;
                t->next = newNode;
                node_inserted = TRUE;
                break;
            }
            t = t->next;
        }
        if(node_inserted == FALSE){
            t->next = newNode;
        }
    }
}

/*swap two tiles in a node*/
void swap(int row1,int column1,int row2,int column2, struct node * pnode){
    int tile = pnode->tiles[row1][column1];
    pnode->tiles[row1][column1] = pnode->tiles[row2][column2];
    pnode->tiles[row2][column2] = tile;
}

/*update the f,g,h function values for a node */
void update_fgh(struct node *pnode){
    /*
        manhattan distance -  a taxicab geometry in which the usual distance function or metric of
        euclidean geometry is replaced by a new metric in which the distance between two points is
        the sum of the absolute differences of their cartesian coordinates

        f(n) = g(n) + h(n)
    */

    //update g value
    if(pnode->parent != NULL){
        pnode->g = pnode->parent->g++;
    }else{
        pnode->g++;
    }

    //h is the max of misplaced tiles or manhattan distance (how far tile is from correct location)
    int misplaced_tiles = 0;
    int manhattan_dist = 0;
    int i, j;
    int element_position = 0;

    //check number of misplaced tiles
    for(i = 0; i < NxN; i++){
        for(j = 0; i < NxN; i++){
            element_position++;

            if(pnode->tiles[j][i] != element_position){
                misplaced_tiles++;
            }

            //check manhattan distance
            if(pnode->tiles[j][i] == 0){
                continue;
            }
            for(int a = 0; a < NxN; a++){
                for(int b = 0; b < NxN; b++){
                    if(goal->tiles[a][b] == pnode->tiles[j][i]){
                        manhattan_dist += abs(j - a) + abs(i - b);
                    }
                }
            }
        }
    }

    if(misplaced_tiles > manhattan_dist){
        pnode->h = misplaced_tiles;
    }else{
        pnode->h = manhattan_dist;
    }

    pnode->f = pnode->g + pnode->h;
}



/* 0 goes down by a row */
void move_down(struct node * pnode){
    if(pnode->zero_row+1 < N){
            swap(pnode->zero_row, pnode->zero_column, pnode->zero_row+1, pnode->zero_column, pnode);
        pnode->zero_row++;
    }else{
        pnode = NULL;
    }
}

/* 0 goes right by a column */
void move_right(struct node * pnode){
    if(pnode->zero_column+1 < N){
        swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column+1, pnode);
        pnode->zero_column++;
    }else{
        pnode = NULL;
    }
}

/* 0 goes up by a row */
void move_up(struct node * pnode){
    if(pnode->zero_row-1 > -1){
        swap(pnode->zero_row, pnode->zero_column, pnode->zero_row-1, pnode->zero_column, pnode);
        pnode->zero_row--;
    }else{
        pnode = NULL;
    }

}

/* 0 goes left by a column */
void move_left(struct node * pnode){
    if(pnode->zero_column-1 > -1) {
        swap(pnode->zero_row, pnode->zero_column, pnode->zero_row, pnode->zero_column - 1, pnode);
        pnode->zero_column--;
    }else{
        pnode = NULL;
    }
}

/* expand a node, get its children nodes, and organize the children nodes using
 * array succ_nodes.
 */
void expand(struct node *selected) {
    //expand node into 4 possible states

    for(int i = 0; i < N; i++){
        succ_nodes[i] = (struct node *) malloc(sizeof(struct node));
        memcpy(succ_nodes[i]->tiles, selected->tiles, NxN*sizeof(int));
        succ_nodes[i]->zero_row = selected->zero_row;
        succ_nodes[i]->zero_column = selected->zero_column;
        succ_nodes[i]->parent = selected;

    }
    //if direction possible for expanded node, swap tile in direction, else NULL expanded node in successor nodes
    move_down(succ_nodes[0]);
    move_right(succ_nodes[1]);
    move_up(succ_nodes[2]);
    move_left(succ_nodes[3]);

    //update all successor nodes f, g, h values
    for(int i = 0; i < N; i++){
        update_fgh(succ_nodes[i]);
    }
}

int nodes_same(struct node *a,struct node *b) {
	int flg=FALSE;
	if (memcmp(a->tiles, b->tiles, sizeof(int)*NxN) == 0)
		flg=TRUE;
	return flg;
}

/* Filtering. Some nodes in succ_nodes may already be included in either open
* or closed list. Remove them. It is important to reduce execution time.
* This function checks the (i)th node in succ_nodes array. You must call this
& function in a loop to check all the nodes in succ_nodes. */
void filter(int i, struct node *pnode_list){
    if(pnode_list == NULL || succ_nodes[i] == NULL){
        return;
    }
    struct node *t = pnode_list;

    while(t != NULL){
        if(nodes_same(succ_nodes[i], t)){
            succ_nodes[i] = NULL;
            return;
        }
        t = t->next;
    }
}

int main(int argc,char **argv) {
	int iter,cnt;
	struct node *copen, *cp, *solution_path;
	int ret, i, pathlen=0, index[N-1];

	solution_path=NULL;
	start=initialize(argv);	/* init initial and goal states */
	open=start;

	iter=0;
	while (open!=NULL) {	/* Termination cond with a solution is tested in expand. */
		copen=open;
		open=open->next;  /* get the first node from open to expand */
		if(nodes_same(copen,goal)){ /* goal is found */
			do{ /* trace back and add the nodes on the path to a list */
				copen->next=solution_path;
				solution_path=copen;
				copen=copen->parent;
				pathlen++;
			} while(copen!=NULL);
			printf("Path (lengh=%d):\n", pathlen);
			copen=solution_path;
			/* print out the nodes on the list */
			do{
			    //traverse solution_path
                print_a_node(copen);
                copen = copen->next;
            }while(copen != NULL);
			break;
		}
		expand(copen);       /* Find new successors */
		for(i=0;i<4;i++){
			filter(i,open);
			filter(i,closed);
		}
		merge_to_open(); /* New open list */
		copen->next=closed;
		closed=copen;		/* New closed */
		iter++;
		if(iter %1000 == 0)
			printf("iter %d\n", iter);
	}
	return 0;
} /* end of main */
