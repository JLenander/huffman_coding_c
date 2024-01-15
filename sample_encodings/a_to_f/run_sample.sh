#! /bin/bash
# Run in the /sample_encodings/a_to_f/ directory
../../encoder -i text.txt -o text.cmp -e encoding -c
../../encoder -i text.cmp -o decompressed.txt -e encoding -d
