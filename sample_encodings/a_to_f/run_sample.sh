#! /bin/bash
../../encoder -i text.txt -o text.cmp -e encoding -c
../../encoder -i text.cmp -o decompressed.txt -e encoding -d
