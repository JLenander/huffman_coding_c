#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "encoding.h"

/*
Parse the int array into an integer encoding.

The entries of <intArr> should be 0 or 1 for the length of the encoding and -1 to indicate
the encoding is finished.

<arrMaxLen> denotes the maximum length of <intArr> and must be at least 1

If we reach the end of the array or the integer bit limit return the encoding up until this point.
*/
int encodingArrToInt(int intArr[], int arrMaxLen) {
    // Ensure we do not exceed the integer limit
    arrMaxLen = (arrMaxLen > sizeof(int) * 8) ? sizeof(int) * 8 : arrMaxLen;

    int encoding = intArr[0];

    for (int i = 1; i < arrMaxLen; i++) {
        if (intArr[i] == -1) {
            return encoding;
        }

        encoding = encoding << 1;
        encoding += intArr[i];
    }

    return encoding;
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
            encoding->encodings[i] = newenc.encodings[i];
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

    // Zeroes the unused entries
    for (int i = encoding.alphabetlen; i < MAX_ALPHABET_LEN; i++) {
        encoding.alphabet[i] = '\0';
        encoding.encodings[i] = -1;
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
