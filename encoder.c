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
    // The file containing the encoding representation.
    FILE *encodingFile;
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
    inputArgs.encodingFile = fopen(encodingFilepath, "rb");
    inputArgs.inputFile = fopen(inputFilepath, "r");
    inputArgs.outputFile = fopen(outputFilepath, "w");

    if (inputArgs.encodingFile == NULL || inputArgs.inputFile == NULL || inputArgs.outputFile == NULL) {
        fprintf(stderr, "Failed to open one or more files\n");
        exit(1);
    }

    return inputArgs;
}

int encode_file(FILE *inputFile, FILE *outputFile, Encoding encoding) {
    return -1;
}

int decode_file(FILE *inputFile, FILE *outputFile, Encoding encoding) {
    return -1;
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

    return 0;
}