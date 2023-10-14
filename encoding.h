// The maximum length of any name used as a descriptor
#define MAX_NAME 32
/* The maximum number of characters in an alphabet.
This number is a bit more than the 104 keys on a standard english keyboard
to allow for the full keyboard character range plus extra characters.
This program is extendable to larger alphabets but would require different
data structure implementation for larger alphabets. */
#define MAX_ALPHABET_LEN 128
// The header is the first HEADER_SIZE bytes that identify the encoding file
#define HEADER_SIZE 5
#define HEADER "HFENC"

/*
The data type used for the encoding of an alphabet.
Denoted by <name>
*/
typedef struct encoding {
    char name[MAX_NAME];
    char alphabet[MAX_ALPHABET_LEN];
    int encodings[MAX_ALPHABET_LEN];
} Encoding;

/*
The data type used for the input to the encoding generation algorithm.
Denoted by <name>
*/
typedef struct frequencies {
    char name[MAX_NAME];
    char alphabet[MAX_ALPHABET_LEN];
    float frequencies[MAX_ALPHABET_LEN];
} Frequencies;

/*
The data type for a tree node (implemented using an array in the algorithm).
*/
typedef struct symbol {
    char character;
    float frequency;
} Symbol;

/*
Load the encoding from <filepath> into <encoding>.
Returns 0 on success.
On error, returns:
    - 1 if the file does not exist or could not be loaded
    - 2 if the file is of invalid type
    - 3 if the encoding could not be loaded for some other reason.

Files with a valid header (First 5 bytes are "HFENC") are assumed to be valid
encodings. No check of data validity is performed besides the header.

The encoding pointed to by <encoding> has all of it's data replaced with the
loaded data.
*/
int load(char *filepath, Encoding *encoding);

/*
Save the <encoding> into the file specified by <filepath>.
Returns 0 on success.
On error, returns:
    - 1 if the file could not be created
    - 2 if the encoding could not be saved for some reaons

The file is saved with the 5-byte header "HFENC" immediately followed by the
Encoding struct data.
*/
int save(char *filepath, Encoding encoding);