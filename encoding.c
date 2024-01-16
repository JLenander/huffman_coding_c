#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "encoding.h"

/*
Construct and return a pointer to a new empty encoding struct with name <name>
*/
Encoding *newEncoding(char *name) {
    Encoding *newEnc = malloc(sizeof(Encoding));
    if (newEnc == NULL) {
        fprintf(stderr, "Failed to allocate memory for new encoding struct\n");
        exit(1);
    }

    strncpy(newEnc->name, name, MAX_NAME);
    newEnc->name[MAX_NAME - 1] = '\0';

    newEnc->alphabetlen = 0;

    for (int i = 0; i < MAX_ALPHABET_LEN; i++) {
        newEnc->alphabet[i] = '\0';
        memset(newEnc->encodings[i], ENC_END, MAX_ENC_SIZE_BITS * sizeof(newEnc->encodings[0]));
    }

    return newEnc;
}

/*
Construct and return a pointer to a new empty frequency struct with name <name>
*/
Frequencies *newFrequencies(char *name) {
    Frequencies *newFreq = malloc(sizeof(Frequencies));
    if (newFreq == NULL) {
        fprintf(stderr, "Failed to allocate memory for new frequency struct\n");
        exit(1);
    }

    strncpy(newFreq->name, name, MAX_NAME);
    newFreq->name[MAX_NAME - 1] = '\0';

    newFreq->alphabetlen = 0;

    for (int i = 0; i < MAX_ALPHABET_LEN; i++) {
        newFreq->alphabet[i] = '\0';
        newFreq->frequencies[i] = 0.0;
    }

    return newFreq;
}

/*
Deconstruct the encoding struct pointed to by the pointer <encodingPtr>
and free memory associated with it

Return 0 on success
Return 1 otherwise
*/
int destroyEncoding(Encoding *encodingPtr) {
    free(encodingPtr);
    return 0;
} 

/*
Deconstruct the frequencies struct pointed to by the pointer <freqPtr>
and free memory associated with it

Return 0 on success
Return 1 otherwise
*/
int destroyFrequencies(Frequencies *freqPtr) {
    free(freqPtr);
    return 0;
}

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
int load(char *filepath, Encoding *encoding) {
    if (access(filepath, R_OK) != 0) {
        return 1;
    }
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        return 1;
    }

    // Checking the header of the file for "HFENC"
    char header[HEADER_SIZE + 1];
    if (fread(header, HEADER_SIZE, 1, file) != 1) {
        // Either failed to read the file or the file did not even contain
        // the expected number header bytes.
        fclose(file);
        return 2;
    }
    // Null terminating the header is technically not necessary with strncmp
    // but it is done anyways for safety
    header[HEADER_SIZE] = '\0';
    if (strncmp(header, HEADER, HEADER_SIZE) != 0) {
        fclose(file);
        return 2;
    }

    Encoding newenc;
    // Load the encoding from the file. The Encoding data should be located
    // directly after the 5-byte header data.
    if (fread(&newenc, sizeof(Encoding), 1, file) == 1) {
        // Success, copy the data over
        strncpy(encoding->name, newenc.name, MAX_NAME);
        encoding->alphabetlen = newenc.alphabetlen;
        for (int i = 0; i < MAX_ALPHABET_LEN; i++) {
            encoding->alphabet[i] = newenc.alphabet[i];
            // encoding->encodings[i] = newenc.encodings[i];
            memcpy(encoding->encodings[i], newenc.encodings[i], sizeof(int) * MAX_ENC_SIZE_BITS);
        }

        fclose(file);
        return 0;
    } else {
        // Failure to read the data
        fclose(file);
        return 3;
    }

}

/*
Save the <encoding> into the file specified by <filepath>.
Returns 0 on success.
On error, returns:
    - 1 if the file could not be created
    - 2 if the encoding could not be saved for some reaons

The file is saved with the 5-byte header "HFENC" immediately followed by the
Encoding struct data. Entries in encoding.alphabet and encoding.encodings
after index alphabetlen - 1 are zeroed.
*/
int save(char *filepath, Encoding encoding) {
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        return 1;
    }

    // Zero the empty entries
    for (int i = encoding.alphabetlen; i < MAX_ALPHABET_LEN; i++) {
        encoding.alphabet[i] = '\0';
        // encoding.encodings[i] = -1;
        for (int j = 0; j < MAX_ENC_SIZE_BITS; j++) {
            encoding.encodings[i][j] = ENC_END;
        }
    }

    // Write the header bytes
    if (fwrite(HEADER, HEADER_SIZE, 1, file) != 1) {
        fclose(file);
        return 2;
    }

    // Write the encoding data
    if (fwrite(&encoding, sizeof(Encoding), 1, file) == 1) {
        // Success
        fclose(file);
        return 0;
    } else {
        // Failure
        fclose(file);
        return 2;
    }
}
