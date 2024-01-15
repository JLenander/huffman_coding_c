#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include "encoding.h"

// Data structure used for the input argument data
typedef struct inputArgData {
    // true if we are compressing the input file. false if we are decompressing.
    bool compressing;
    FILE *inputFile;
    FILE *outputFile;
    // The filepath containing the encoding representation.
    char *encodingFilepath;
} InputArgData;

/*
Parse the program input arguments and verify input validity.
Returns a struct representing the input argument data.
*/
InputArgData parse_input_args(int argc, char **argv) {
    // The string used in error messages related to invalid input arguments.
    char *INPUT_ERR_STR = "Usage: %s -i <input_file> -e <encoding_file> (-c|-d) [-o <output_file>]\n";

    // If called with no arguments, print usage string.
    if (argc == 1) {
        fprintf(stderr, INPUT_ERR_STR, argv[0]);
        exit(1);
    }

    // Load the options
    char opt;  // stores the input character
    char *inputFilepath = "";
    char *encodingFilepath = "";
    int compressing = -1; // > 0 if we are compressing the file, = 0 if we are decompressing the file
    // Optional arguments
    char *outputFilepath = "";

    // sets a flag to stop getopt from printing an error message on invalid option.
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:o:e:cd")) != -1) {
        switch (opt) {
            case 'i':
                inputFilepath = strdup(optarg);
                if (inputFilepath == NULL) {
                    exit(2);
                }
                break;
            case 'e':
                encodingFilepath = strdup(optarg);
                if (encodingFilepath == NULL) {
                    exit(2);
                }
                break;
            case 'o':
                outputFilepath = strdup(optarg);
                if (outputFilepath == NULL) {
                    exit(2);
                }
                break;
            case 'c':
                compressing = 1;
                break;
            case 'd':
                compressing = 0;
                break;
            default:
                fprintf(stderr, INPUT_ERR_STR, argv[0]);
                exit(1);
        }
    }

    // Test that all required variables were set and given.
    // The user must specify if they are compressing or decompressing the file
    if (compressing == -1) {
        fprintf(stderr, "Required: specify compressing with -c or decompressing with -d\n");
        exit(1);
    }

    // The string variables are all initialized as empty strings.
    if (inputFilepath[0] == '\0' || encodingFilepath[0] == '\0') {
        fprintf(stderr, INPUT_ERR_STR, argv[0]);
        exit(1);
    }

    // If the output filepath was not provided, generate it from the input filepath.
    // The default is the input file appended with ".cmp" for compressing
    // or the input file appended with ".txt" for decompressing
    if (outputFilepath[0] == '\0') {
        if (compressing) {
            int inputFileLen = strlen(inputFilepath);
            // +5 accounts for the ".cmp" and the null terminating byte
            outputFilepath = malloc(inputFileLen + 5);
            if (outputFilepath == NULL) {
                exit(1);
            }

            // The inputfilepath is null terminated so we add + 1 to copy that null terminator over to the 
            // output filepath. This sets up the call to strncat which expects a null terminated dest str.
            strncpy(outputFilepath, inputFilepath, inputFileLen + 1);
            strncat(outputFilepath, ".cmp", 5);
        } else {
            // Will be changed in the future to replace ".cmp" if it exists
            int inputFileLen = strlen(inputFilepath);
            // +5 accounts for the ".txt" and the null terminating byte
            outputFilepath = malloc(inputFileLen + 5);
            if (outputFilepath == NULL) {
                exit(1);
            }

            // The inputfilepath is null terminated so we add + 1 to copy that null terminator over to the 
            // output filepath. This sets up the call to strncat which expects a null terminated dest str.
            strncpy(outputFilepath, inputFilepath, inputFileLen + 1);
            strncat(outputFilepath, ".txt", 5);
        }

    }

    // Check that files exist
    if (access(inputFilepath, F_OK) != 0) {
        fprintf(stderr, "Invalid input file (does not exist)\n");
        exit(1);
    }
    if (access(encodingFilepath, F_OK) != 0) {
        fprintf(stderr, "Invalid encoding file (does not exist)\n");
        exit(1);
    }

    InputArgData inputArgs;

    inputArgs.compressing = compressing;
    inputArgs.inputFile = fopen(inputFilepath, "r");
    inputArgs.outputFile = fopen(outputFilepath, "w");
    inputArgs.encodingFilepath = strdup(encodingFilepath);

    if (inputArgs.encodingFilepath == NULL) {
        fprintf(stderr, "Failed to allocate memory for encoding filepath\n");
        exit(1);
    }

    if (inputArgs.inputFile == NULL || inputArgs.outputFile == NULL) {
        fprintf(stderr, "Failed to open one or more files\n");
        exit(1);
    }

    return inputArgs;
}

/*
Returns the ith bit of the <buffer>
*/
int get_ith_bit(unsigned char buffer[MAX_ENC_SIZE_BITS * 2], int i) {
    return (buffer[i / 8] >> i % 8) & 1;
}

/*
Set the ith bit of the <buffer> to <bit>
*/
void set_ith_bit(unsigned char buffer[MAX_ENC_SIZE_BITS * 2], int i, int bit) {
    if (bit) {
        // Bit exists, ensure bit is set to 1
        buffer[i / 8] = buffer[i / 8] | (1 << i % 8);
    } else {
        // Bit is zero, ensure bit is set to 0
        buffer[i / 8] = buffer[i / 8] & (255 - (1 << i % 8));
    }
}

/*
Insert the integer <integer> into the bit buffer starting at bit <i>.
*/
void insert_int_by_bit(unsigned int integer, char buffer[MAX_ENC_SIZE_BITS * 2], int i) {
    // j is the relative index of the bit starting at i
    for (int j = 0; j < sizeof(int) * 8; j++) {
        // The absolute index of the bit
        int bit_index = j + i;
        // The bit to insert
        int bit = (integer >> j) & 1;

        buffer[bit_index] = bit;
    }
}

/*
Returns the number of bits in the encoding array.
This is the number of entries before we reach ENC_END.
*/
int num_bits(int enc[MAX_ENC_SIZE_BITS]) {
    int num_bits = 0;
    for (int i = 0; i < MAX_ENC_SIZE_BITS; i++) {
        if (enc[i] == ENC_END) {
            return num_bits;
        }
        num_bits++;
    }
    return num_bits;
}

/*
Given a plaintext <inputFile> and an <encoding>, encode the input file.

The characters are encoded in 8 bit buffers

Returns 0 on success.
Returns 1 if a character that is not in the encoding alphabet is encountered.
Returns 2 if there was an error writing to the <outputFile>
*/
int encode_file(FILE *inputFile, FILE *outputFile, Encoding encoding) {
    // We use buffer to store up to 1 byte at a time until we can write a byte to the file.
    int buffer[8];
    // The index of the buffer to write the next bit to
    int bufferi = 0;

    char nextChar = '\0';
    while ((nextChar = fgetc(inputFile)) != EOF) {
        for (int i = 0; i <= encoding.alphabetlen; i++) {
            if (i == encoding.alphabetlen) {
                // Did not find the character in the alphabet
                return 1;
            }

            if (encoding.alphabet[i] == nextChar) {
                int charEnc[MAX_ENC_SIZE_BITS];
                for (int j = 0; j < MAX_ENC_SIZE_BITS; j++) {
                    charEnc[j] = encoding.encodings[i][j];
                }

                // The index of the bit in charEnc to write to the buffer
                // starting at the lowest bit until we've written the whole encoding.
                // If the encoding is more than a byte in length, the buffer will be written
                // out to the file.
                int numBits = num_bits(charEnc);
                int biti = 0;
                do {
                    buffer[bufferi] = charEnc[biti];
                    biti++;
                    bufferi++;

                    // Write the bits out if we've filled the buffer
                    if (bufferi == 8) {
                        bufferi = 0;
                        
                        // Construct a 8-bit sequence in a char type
                        unsigned char bufferChar = 0;

                        for (int j = 0; j < 8; j++) {
                            // We bitshift the buffer by the appropriate bits
                            bufferChar += buffer[j] << j;
                        }

                        // Write out the corresponding encoding.
                        if (fwrite(&bufferChar, 1, 1, outputFile) != 1) {
                            // Failed to write the character
                            return 2;
                        }
                    }
                } while (biti < numBits);

                // We've found and written the character so break out of the for loop.
                break;
            }
        }
    }

    // Write the remaining bits out with 0 as padding and write out the footer
    char numPaddingBits = 0;
    char bufferChar = 0;
    for (int i = bufferi; i < 8; i++) {
        buffer[i] = 0;
        numPaddingBits++;
    }
    for (int j = 0; j < 8; j++) {
        bufferChar += buffer[j] << j;
    }

    // Write out the corresponding padded encoding for the last character
    if (fwrite(&bufferChar, 1, 1, outputFile) != 1) {
        // Failed to write the character
        return 2;
    }

    // Write out the footer with the number of padding bits used
    if (fwrite(&numPaddingBits, FOOTER_SIZE, 1, outputFile) != 1) {
        // Failed to write footer
        return 2;
    }

    return 0;
}

/*
Helper for decode_buffer().
Compares the <charEncArr> and the <intEncArr> to see if they contain the same encoding.
Two encodings are considered the same if they have the n elements in common followed by a single
ENC_END element (this means elements after the first ENC_END encountered are not checked).

Returns 0 if the buffers do not contain the same encoding.
Returns 1 if the buffers contain the same encoding.
*/
int sameEncoding(int intEncArr[MAX_ENC_SIZE_BITS], char charEncArr[MAX_ENC_SIZE_BITS]) {
    for (int i = 0; i < MAX_ENC_SIZE_BITS; i++) {
        if (intEncArr[i] != charEncArr[i]) {
            return 0;
        }

        if (charEncArr[i] == ENC_END) {
            return 1;
        }
    }

    return 1;
}

/*
Helper for decode_file().
Decode the bits in the 2*MAX_ENC_SIZE_BITS sized <buffer>.
Stores the index of the start of the current encoding we are examining in 
<currEncodingIndex>.
Writes recognized characters to <outputFile>.
Uses Encoding <encoding>.

Returns 0 on success.
Returns 1 if an encoded character that is not in the encoding alphabet is encountered.
Returns 2 if the output file cannot be written to.
*/
int decode_buffer(char buffer[MAX_ENC_SIZE_BITS * 2], int *currEncodingIndex, int bufferEnd,
                   FILE *outputFile, Encoding encoding) {
    // encArr holds the buffered encoding bits as we match encArr with encodings in <encoding>
    char encArr[MAX_ENC_SIZE_BITS];
    for (int i = 0; i < MAX_ENC_SIZE_BITS; i++) {
        encArr[i] = ENC_END;
    }

    for (int bufferi = 0; bufferi < bufferEnd; bufferi++) {
        int encArri = bufferi - *currEncodingIndex;
        if (encArri == MAX_ENC_SIZE_BITS) {
            // Maximum encoding size reached, this encoding is not valid
            return 1;
        }

        // Add the ith bit in buffer into the buffered encoding <enc>
        encArr[encArri] = buffer[bufferi];

        // Search for the encoding in the alphabet
        for (int encodingsi = 0; encodingsi < encoding.alphabetlen; encodingsi++) {
            if (encArr[encArri] == ENC_END) {
                break;
            }

            if (sameEncoding(encoding.encodings[encodingsi], encArr)) {
                // Found character, write out to file
                if (fwrite(&encoding.alphabet[encodingsi], 1, 1, outputFile) != 1) {
                    return 2;
                }

                // reset the buffered encoding back to its empty state
                for (int j = 0; j < MAX_ENC_SIZE_BITS; j++) {
                    encArr[j] = ENC_END;
                }
                // set the new start of the next encoding
                *currEncodingIndex = bufferi + 1;

                break;
            }
        }
    }

    return 0;
}

/*
Given a compressed <inputFile> and an <encoding>, decode the input file.

Returns 0 on success.
Returns 1 if an encoded character that is not in the encoding alphabet is encountered.
Returns 2 if there was an error writing to the <outputFile>
Returns 3 if there was an error reading from <inputFile>
*/
int decode_file(FILE *inputFile, FILE *outputFile, Encoding encoding) {
    // The footer of the file stores the number of padding bits we used.
    if (fseek(inputFile, -1 * FOOTER_SIZE, SEEK_END) == -1) {
        return 3;
    }
    // Stores the byte position of beginning of the last body content byte
    long lastContentByte = ftell(inputFile) - 1;

    FOOTER_TYPE numPaddingBits = 0;
    if (fread(&numPaddingBits, FOOTER_SIZE, 1, inputFile) == -1) {
        return 3;
    }
    rewind(inputFile);

    // We keep a bit buffer for the input.
    // (an individual character can be encoded in at most MAX_ENC_SIZE_BYTES bytes
    // and so we keep 2 * MAX_ENC_SIZE_BITS bits for overflow purposes)
    char buffer[MAX_ENC_SIZE_BITS * 2];

    // Initialize the buffer using the ENC_END character
    for (int i = 0; i < MAX_ENC_SIZE_BITS * 2; i++) {
        buffer[i] = ENC_END;
    }

    // Read in MAX_ENC_SIZE_BYTES bytes into the buffer starting at the next available bit
    unsigned int readbuffer = 0;
    // The index after the last bit of data in the buffer.
    int bufferEnd = 0;
    // Keeps track of how many bytes were read for the last call
    int bytesRead = 0;
    // Loop until we reach the last content byte or some read issue occurs.
    for (int currByte = 0; currByte < lastContentByte; currByte++) {
        // Read in the next byte
        if ((bytesRead = fread(&readbuffer, 1, 1, inputFile)) != 1) {
            // Some read error occurred or unexpected EOF
            return 3;
        }

        // Insert the readbuffer into the correct bits in the bit buffer <buffer>
        insert_int_by_bit(readbuffer, buffer, bufferEnd);
        bufferEnd += bytesRead * 8;

        // Greedily decode the buffered bits
        // holds the index of the start of the current encoding for shifting the buffer over
        int currEncodingIndex = 0;
        
        // Decode all character encodings present in the buffer
        int decode_ret = decode_buffer(buffer, &currEncodingIndex, bufferEnd, outputFile, encoding);
        if (decode_ret != 0) {
            return decode_ret;
        }

        // Shift all leftover bits to the beginning of the buffer.
        // Pad any extra space with ENC_END.
        int leftover_bit_count = 0;
        // ri tracks the index of the leftover bits to copy over to the left
        int ri = currEncodingIndex;
        // li tracks the index to copy the leftover bits to (from the left)
        for (int li = 0; li < (MAX_ENC_SIZE_BITS * 2); li++) {
            // If there are no more leftover bits, we set the lith bit to ENC_END;
            char leftover_bit = ENC_END;
            if (ri < bufferEnd) {
                // There are still leftover bits to read
                leftover_bit = buffer[ri];
                leftover_bit_count++;
            }

            buffer[li] = leftover_bit;
            ri++;
        }
        bufferEnd = leftover_bit_count;
    }

    // Check if we exited the loop because there was a file read error or the file ended unexpectedly
    // (otherwise we exited because we reached the last content byte position.
    if (ferror(inputFile) || feof(inputFile)) {
        return 3;
    }

    // We are just before the last content byte. Read this content byte in and turn any padding
    // bits into ENC_END before decoding the final content byte.
    if ((bytesRead = fread(&readbuffer, 1, 1, inputFile)) != 1) {
        return 3;
    }

    insert_int_by_bit(readbuffer, buffer, bufferEnd);
    bufferEnd += (bytesRead * 8);

    // Turn the last numPaddingBits into ENC_END as they were padding.
    for (int i = bufferEnd - 1; i > bufferEnd - numPaddingBits - 1; i--) {
        buffer[i] = ENC_END;
    }
    bufferEnd -= numPaddingBits;

    // Decode the last byte as normal
    int currEncodingIndex = 0;
    int decode_ret = decode_buffer(buffer, &currEncodingIndex, bufferEnd, outputFile, encoding);
    return decode_ret;
}

/* This program reads a text file and compresses or decompresses the file as specified

Options:
    "-i" : Specifies the input file (REQUIRED)
    "-o" : Specifies the output file (by default outputs the encoded file in the
           same directory as the original file as filename.enc)
    "-e" : Specifies the compression encoding to use for this file (REQUIRED)
    "-c" : Specifies that the input file should be compressed   (-c or -d is REQUIRED)
    "-d" : Specifies that the input file should be decompressed (-c or -d is REQUIRED)
*/
int main(int argc, char **argv) {
    InputArgData inputData = parse_input_args(argc, argv);

    Encoding encoding;

    load(inputData.encodingFilepath, &encoding);

    if (inputData.compressing) {
        return encode_file(inputData.inputFile, inputData.outputFile, encoding);
    } else {
        return decode_file(inputData.inputFile, inputData.outputFile, encoding);
    }

    return 0;
}