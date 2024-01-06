# Lossless Compression Implementation

A C implementation of Huffman codes for lossless compression of text.

The lossless compression works with an input of characters and their frequencies.

The `create_encoding` algorithm first creates a prefix-free bit encoding of the characters in order to reduce their size.
This algorithm uses a priority queue which is implemented in [`priority_queue.c`](priority_queue.c)
with a min-heap structure implemented with an array.

The `encode_file` algorithm in [`encoder.c`](encoder.c) takes text in and outputs an encoded (compressed) bitstream

The `decode_file` algorithm in [`encoder.c`](encoder.c) takes in the encoded bitstream and outputs text.

Since the smallest unit of any data type is 1 byte in c (and in most file systems), several helper functions
exist to help with bit manipulation (ex. `set_ith_bit` in [`encoder.c`](encoder.c)). The `encode_file` and `decode_file`
algorithms keep buffers of `unsigned char` that bits are stored in before being written to the file or
being decoded into text.

## Examples
#### Small encoding consisting of characters {a,b,c,d,e,f,\n}
- [original text](/sample_encodings/a_to_f/text.txt)
- [compressed text](/sample_encodings/a_to_f/text.cmp) (not human readable)
- [encoding file](/sample_encodings/a_to_f/encoding) (not human readable)
- [decompressed file](/sample_encodings/a_to_f/decompressed.txt)
- [small script to run the compression and decompression](/sample_encodings/a_to_f/run_sample.sh)

The uncompressed file (`text.txt`) takes up 19 bytes while the compressed file (`test.cmp`) takes up 10 bytes.
![image showing the uncrompressed file takes up 19 bytes while the compressed file takes up 10 bytes](/imgs/a_to_f_size.png)
Hex values of the corresponding files
![image showing hexadecimal encoding of the plaintext and compressed files](/imgs/a_to_f_hex.png)
