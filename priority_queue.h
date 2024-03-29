/*
A Tree node for a binary tree
If left and right are NULL pointers, this node is a leaf node
*/
typedef struct tree {
    char symbol;
    struct tree *left;
    struct tree *right;
} Tree;

/*
A QueueItem is an item in a priority queue
Empty is 1 if this QueueItem should be considered empty or NULL in the queue structure
Empty is 0 if this QueueItem contains data as part of the queue structure
Every QueueItem corresponds to a node in a binary tree and so has a pointer to that tree node
*/
typedef struct queue_item {
    char symbol;
    float weight;
    int empty;
    Tree *treeNode;
} QueueItem;

/*
The priority queue data structure.

maxQueueLength is the maximum elements this queue can support
length is the current number of elements in the queue.
*/
typedef struct priority_queue {
    QueueItem *queue;
    int maxQueueLength;
    int length;
} PriorityQueue;

/*
Initializes and returns a pointer to a new Tree with <symbol> and no children.
*/
Tree *newTree(char symbol);

/*
Initializes and returns a new nonempty QueueItem
If <treeNode> is NULL, a new Tree struct will be created with <symbol>
*/
QueueItem newQueueItem(char symbol, float weight, Tree *treeNode);

/*
Initializes and returns a pointer to a new priority queue
*/
PriorityQueue *newQueue(int maxQueueLength);

/*
Enqueue method for the priority queue

Returns 0 if successful
Returns 1 if queue is full
*/
int enqueue(PriorityQueue *pqueue, QueueItem item);

/*
Dequeue method for the priority queue

Remove and return the first item in the priority queue
The priority queue must be non empty
*/
QueueItem dequeue(PriorityQueue *pqueue);