#include "encoding.h"
#include "priority_queue.h"

/*
Run the Huffman Coding algorithm to create a prefix-free encoding given
a set of symbols and associated frequencies
*/
Encoding *generateEncoding(Frequencies freqs, char encodingName[MAX_NAME]);