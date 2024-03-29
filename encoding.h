// The maximum length of any name used as a descriptor
#define MAX_NAME 32
/* The maximum number of characters in an alphabet.
This number is a bit more than the 104 keys on a standard english keyboard
to allow for the full keyboard character range plus extra characters.
This program is extendable to larger alphabets but would require different
data structure implementation for larger alphabets. */
#define MAX_ALPHABET_LEN 128
// The header is the first HEADER_SIZE bytes that identify the *encoding* file
#define HEADER_SIZE 5
#define HEADER "HFENC"
// The maximum size of any single encoding in *bits*
#define MAX_ENC_SIZE_BITS 32
// The maximum size of any single encoding in *bytes*
#define MAX_ENC_SIZE_BYTES 4
// Used to mark the end of an encoding in the encoding integer array
#define ENC_END -1
// The footer is the last FOOTER_SIZE bytes in the *compressed* file that contain extra data
// (like the number of padding bits in the last encoded character)
#define FOOTER_SIZE 1
// The data type used for the footer (the last number of padding bits will be between 0 and 7)
#define FOOTER_TYPE char

/*
The data type used for the encoding of an alphabet.
Denoted by <name>.
<alphabetlen> is the length of the alphabet.
*/
typedef struct encoding {
    char name[MAX_NAME];
    int alphabetlen;
    char alphabet[MAX_ALPHABET_LEN];
    int encodings[MAX_ALPHABET_LEN][MAX_ENC_SIZE_BITS];
} Encoding;

/*
The data type used for the input to the encoding generation algorithm.
Denoted by <name>.
<alphabetlen> is the length of the alphabet
*/
typedef struct frequencies {
    char name[MAX_NAME];
    int alphabetlen;
    char alphabet[MAX_ALPHABET_LEN];
    float frequencies[MAX_ALPHABET_LEN];
} Frequencies;

/*
Construct and return a pointer to a new empty encoding struct with name <name>
*/
Encoding *newEncoding(char *name);

/*
Construct and return a pointer to a new empty frequency struct with name <name>
*/
Frequencies *newFrequencies(char *name);

/*
Deconstruct the encoding struct pointed to by the pointer <encodingPtr>
and free memory associated with it

Return 0 on success
Return 1 otherwise
*/
int destroyEncoding(Encoding *encodingPtr);

/*
Deconstruct the frequencies struct pointed to by the pointer <freqPtr>
and free memory associated with it

Return 0 on success
Return 1 otherwise
*/
int destroyFrequencies(Frequencies *freqPtr);

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