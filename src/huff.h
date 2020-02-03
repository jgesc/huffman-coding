#ifndef __HUFF_H__
#define __HUFF_H__

#include <stdio.h>
#include <stdlib.h>

/**
void huff_cod(FILE * in, FILE * out)

Compresses a file using Huffman Coding algorithm.

Arguments:
	in: file stream to compress
   out: compressed file stream
**/
void huff_cod(FILE * in, FILE * out);

/**
void huff_dec(FILE * in, FILE * out)

Decodes a byte stream using Huffman Coding algorithm.

Arguments:
	in: compressed file to decode
   out: original file
**/
void huff_dec(FILE * in, FILE * out);;

#endif