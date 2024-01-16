#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman_coding.h"

/*
Helper method for generateEncoding.

Traverses the binary tree with root at <root> and fills the <encoding> with the prefix-free
encoding and alphabet details.

<currEncoding> is an array specifying the current prefix-free encoding based on the branches we are
on with index 0 being the leftmost digit of the encoding and -1 indicating the end of the current
encoding.

<encodingCurrPos> is the index of the current branching part of the <currEncoding> array and is
equal to the depth we are at in the binary tree.
*/
void traverseEncodingTree(Encoding *encoding, Tree *root, int currEncoding[], int encodingCurrPos) {
    if (root->symbol != '\0') {
        // We've reached a leaf node containing a real symbol instead of a dummy symbol
        // Add this symbol to the next available spot and record the encoding associated with it
        encoding->alphabet[encoding->alphabetlen] = root->symbol;
        // Copy the encoding bits over
        for (int i = 0; i < MAX_ENC_SIZE_BITS; i++) {
            encoding->encodings[encoding->alphabetlen][i] = currEncoding[i];
        }
        encoding->alphabetlen++;

        return;
    }

    // Recursively examine the left and right children
    // Left branch is 0
    currEncoding[encodingCurrPos] = 0;
    traverseEncodingTree(encoding, root->left, currEncoding, encodingCurrPos + 1);
    // Right branch is 1
    currEncoding[encodingCurrPos] = 1;
    traverseEncodingTree(encoding, root->right, currEncoding, encodingCurrPos + 1);
    // After doing the recursive calls, set the currEncoding for this branch back to -1
    // so that other recursive calls at this depth have the correct encoding.
    currEncoding[encodingCurrPos] = -1;
}

/*
Run the Huffman Coding algorithm to create a prefix-free encoding given
a set of symbols and associated frequencies

There must be at least two symbols in <freqs>
*/
Encoding *generateEncoding(Frequencies freqs, char encodingName[MAX_NAME]) {
    // Construct the priority queue form the symbols in freqs
    // Each symbol's weight is the frequency provided
    PriorityQueue *pqueue = newQueue(freqs.alphabetlen);
    for (int i = 0; i < freqs.alphabetlen; i++) {
        // Method newQueueItem does not malloc a new queueitem struct so this calling method
        // does not create a memory leak by passing the struct into enqueue
        enqueue(pqueue, newQueueItem(freqs.alphabet[i], freqs.frequencies[i], NULL));
    }

    /*
    Main Huffman Coding algorithm:
    - While the queue contains at least 2 items, take the two items with the lowest weight
      out of the queue.
    - Create a new tree node with the two items as the children and the symbol '\0' (Null
      terminator will be used to indicate dummy symbol as this symbol should not exist in
      any alphabet the user provides).
    - If the queue now contains 0 items, the tree node we just created is the root of the
      prefix-free encoding tree we created so we parse this to create the encoding.
    - Otherwise (queue nonempty) we insert a new QueueItem into the priority queue with the combined
      weight of the two symbols or dummy symbols we took out and with a pointer to the tree node
      we created. This QueueItem will have symbol '\0' like the tree node to indicate that it is
      a dummy symbol.
    */
    Tree *lastTreeNodeCreated = NULL;
    do {
        QueueItem item1 = dequeue(pqueue);
        QueueItem item2 = dequeue(pqueue);

        lastTreeNodeCreated = newTree('\0');
        lastTreeNodeCreated->left = item1.treeNode;
        lastTreeNodeCreated->right = item2.treeNode;

        enqueue(pqueue, newQueueItem('\0', item1.weight + item2.weight, lastTreeNodeCreated));
    } while (pqueue->length >= 2);

    // If the last tree node created is a NULL pointer, the do-while loop didn't run (less than 2
    // elements) or something went wrong with a Malloc call and wasn't caught in the allocation
    // method for a new Tree
    if (lastTreeNodeCreated == NULL) {
        fprintf(stderr, "Invalid alphabet length or error allocating space for new Tree\n");
    }

    /*
    Parse the tree into a prefix-free encoding as follows:
    Starting at the root, every left branch corresponds to a 0
    and every right branch corresponds to a 1 for a symbol's encoding
    Perform a binary tree traversal and keep track of which branch is taken for each symbol
    */
    Encoding *encoding = newEncoding(encodingName);

    int currEncoding[MAX_ENC_SIZE_BITS];
    for (int i = 0; i < MAX_ENC_SIZE_BITS; i++) {
        currEncoding[i] = ENC_END;
    }
    traverseEncodingTree(encoding, lastTreeNodeCreated, currEncoding, 0);

    return encoding;
}
