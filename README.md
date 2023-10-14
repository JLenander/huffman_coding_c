# Lossless Compression Implementation

A proof and C implementation of Huffman codes for lossless compression of text.

The lossless compression works with an input of characters and their frequencies.

The `create_encoding` algorithm first creates a prefix-free bit encoding of the characters in order to reduce their size.

The `encode` algorithm takes text in and outputs an encoded (compressed) bitstream

The `decode` algorithm takes in the encoded bitstream and outputs text.
