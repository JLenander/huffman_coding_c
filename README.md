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

## Compressed File Details
### The Encoding created has the following specification:
- body containing content bytes
    - Last compressed character may be less than a full byte which is smaller than the minimum
      size the OS handles (one byte) so it is padded with `n` trailing zeros until it is a full byte
      (8 bits) in length
- [`FOOTER_SIZE`](encoding.h) (1) byte file footer containing `n`, the number of trailing zeros
  used as padding in the last encoded character

## Encoding notes
### Change in encoding with commit d3646b4
The Encoding data structure defined in [`encoding.h`](encoding.h) was changed with commit [d3646b4](https://github.com/JLenander/huffman_coding_c/commit/d3646b48fa4f5123156e2e7a5166fcc7be7d10f2)

While the old structure was more space and runtime efficient (with bitwise operations and single
integer comparisons to handle comparing encodings) the old format did not allow for leading zeros
in an encoding

(Encoding leading zeros in a binary format would result in `011` evaluating to the
integer `3` which is the same as the binary encoding `11` even though these are both valid
prefix-free encodings in this context)

Allowing for leading zeros in the encoding increases the number of available prefix-free encodings
for an alphabet which decreases the average binary encoding size (number of bits).

The increase in compression rate was why I made the choice to refactor the code and change the format.

The previous format and bitwise manipulation can be viewed in [this prior state](https://github.com/JLenander/huffman_coding_c/tree/56131b92b1cceee0cf663af51ffd542873b2675f)

## Examples
#### Small encoding consisting of characters {a,b,c,d,e,f,\n}
- [original text](/sample_encodings/a_to_f/text.txt)
- [compressed text](/sample_encodings/a_to_f/text.cmp) (not human readable)
- [encoding file](/sample_encodings/a_to_f/encoding) (not human readable)
- [decompressed file](/sample_encodings/a_to_f/decompressed.txt)
- [small script to run the compression and decompression](/sample_encodings/a_to_f/run_sample.sh)

The uncompressed file (`text.txt`) takes up 16 bytes while the compressed file (`test.cmp`) takes up 9 bytes.
![image showing the uncrompressed file takes up 16 bytes while the compressed file takes up 9 bytes](/imgs/a_to_f_size.png)
Hex values of the corresponding files
![image showing hexadecimal encoding of the plaintext files](/imgs/a_to_f_hex.png)
![image showing hexadecimal encoding of the compressed file](/imgs/a_to_f_hex_cmp.png)
