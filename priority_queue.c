#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"

/*
Initializes and returns a pointer to a new Tree with <symbol> and no children.
*/
Tree *newTree(char symbol) {
    Tree *tree = malloc(sizeof(Tree));
    if (tree == NULL) {
        fprintf(stderr, "Failed to allocate memory for a new Tree in a QueueItem\n");
        exit(1);
    }

    tree->symbol = symbol;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

/*
Initializes and returns a new nonempty QueueItem.
If <treeNode> is NULL, a new Tree struct will be allocated with <symbol> and no children
*/
QueueItem newQueueItem(char symbol, float weight, Tree *treeNode) {
    QueueItem item;
    item.symbol = symbol;
    item.weight = weight;
    item.empty = 0;
    if (treeNode == NULL) {
        item.treeNode = newTree(symbol);
    } else {
        item.treeNode = treeNode;
    }
    return item;
}

/*
Initializes and returns a pointer to a new priority queue
*/
PriorityQueue *newQueue(int maxQueueLength) {
    PriorityQueue *pqueue = malloc(sizeof(PriorityQueue));
    if (pqueue == NULL) {
        fprintf(stderr, "Failed to allocate memory for a priority queue\n");
        exit(1);
    }

    pqueue->maxQueueLength = maxQueueLength;
    pqueue->length = 0;
    pqueue->queue = malloc(sizeof(QueueItem) * maxQueueLength);
    if (pqueue->queue == NULL) {
        fprintf(stderr, "Failed to allocate memory for new priority queue array\n");
        exit(1);
    }

    // Initialize every queueitem. 
    for (int i = 0; i < maxQueueLength; i++) {
        pqueue->queue[i].symbol = '\0';
        pqueue->queue[i].weight = 0.0;
        pqueue->queue[i].empty = 1;
        pqueue->queue[i].treeNode = NULL;
    }

    return pqueue;
}

/*
Enqueue method for the priority queue

Returns 0 if successful
Returns 1 if queue is full
*/
int enqueue(PriorityQueue *pqueue, QueueItem item) {
    // Min-Heap based implementation

    // Check if queue is full (our use case should never encounter
    // this as we only reduce the queue length during the
    // prefix-free encoding algorithm)
    if (pqueue->length == pqueue->maxQueueLength) {
        return 1;
    }

    // Add the item to the next empty slot
    int i = 0;
    while (i < pqueue->maxQueueLength) {
        if (pqueue->queue[i].empty) {
            break;
        }
        i++;
    }

    if (i == pqueue->maxQueueLength) {
        return 1;
    }

    // Set this empty slot to the item
    pqueue->queue[i] = item;

    // Move the new item up in the heap until the heap is satisfied.
    while (i > 0) {
        // (Parent of child at index i is at index (i - 1) floor division by 2)
        // C positive integer division is floor division
        int parenti = (i - 1) / 2;
        if (pqueue->queue[parenti].weight > item.weight) {
            // Swap the parent and the child
            pqueue->queue[i] = pqueue->queue[parenti];
            pqueue->queue[parenti] = item;
            i = parenti;
        } else {
            // Heap is now satisfied
            break;
        }
    }

    pqueue->length++;

    return 0;
}

/*
Dequeue method for the priority queue

Remove and return the first item in the priority queue
The priority queue must be non empty
*/
QueueItem dequeue(PriorityQueue *pqueue) {
    if (pqueue->length == 0) {
        fprintf(stderr, "Priority Queue of length 0 passed to dequeue method\n");
        exit(1);
    }

    QueueItem item = pqueue->queue[0];
    pqueue->length--;

    // Move items up the min-heap as necessary
    int i = 0;
    while (i < pqueue->maxQueueLength) {
        int lchildi = 2 * i + 1;
        int rchildi = 2 * i + 2;

        // Conditions for if the left and right children are out of bounds (OOB) or empty
        // The Empty variables are set to true if the child is out of bounds
        int lchildOOB = lchildi >= pqueue->maxQueueLength;
        int rchildOOB = rchildi >= pqueue->maxQueueLength;
        int lchildEmpty = lchildOOB ? 1 : pqueue->queue[lchildi].empty;
        int rchildEmpty = rchildOOB ? 1 : pqueue->queue[rchildi].empty;


        if (lchildOOB
            || (rchildOOB && lchildEmpty)
            || (lchildOOB && rchildOOB)
            || (lchildEmpty && rchildEmpty)) {
            // Left child is out of bounds or
            // Right child is out of bounds and left child is an empty structure or
            // Both children are out of bounds or
            // Both children are empty (this is a leaf node)
            // so we have reached the bottom of the heap
            return item;
        }

        if (rchildOOB) {
            // Right child is out of bounds but left child isn't and is nonempty
            // so we move the left child up
            pqueue->queue[i] = pqueue->queue[lchildi];
            pqueue->queue[lchildi].empty = 1;
            i = lchildi;

            // Since the right child is out of bounds, the left child cannot have children
            return item;
        }

        // Both children are in bounds so if child A is an empty node, swap the parent with child B
        if (rchildEmpty) {
            pqueue->queue[i] = pqueue->queue[lchildi];
            pqueue->queue[lchildi].empty = 1;
            i = lchildi;
        } else if (lchildEmpty) {
            pqueue->queue[i] = pqueue->queue[rchildi];
            pqueue->queue[rchildi].empty = 1;
            i = rchildi;
        } else {
            // Both children are in bounds and nonempty
            // so we need to check which one has the lower weight
            if (pqueue->queue[lchildi].weight < pqueue->queue[rchildi].weight) {
                pqueue->queue[i] = pqueue->queue[lchildi];
                pqueue->queue[lchildi].empty = 1;
                i = lchildi;
            } else {
                pqueue->queue[i] = pqueue->queue[rchildi];
                pqueue->queue[rchildi].empty = 1;
                i = rchildi;
            }
        }        
        // Since both children were in bounds and nonempty, we may not be at the bottom of the heap
    }

    return item;
}