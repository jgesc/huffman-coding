# huffman-coding
My implementation of Huffman Coding compression algorithm made in C to compress and decompress files.

## Reference

### void huff_cod(FILE * in, FILE * out)
Compresses a file using Huffman Coding algorithm.
Arguments:
	in: file stream to compress
   out: compressed file stream

### void huff_dec(FILE * in, FILE * out)
Decodes a byte stream using Huffman Coding algorithm.
Arguments:
	in: compressed file to decode
   out: original file
