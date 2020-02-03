#include "huff.h"

int main(int argv, char** argc)
{
	if(argv != 4)
	{
		printf("Uso: %s [c/d] input output\n", argc[0]);
		return 0;
	}
	
	FILE * in, * out;
	
	if(!(in = fopen(argc[2], "rb"))) 
	{
		perror("ERRROR opening input file");
		return -1;
	}
	if(!(out = fopen(argc[3], "wb")))
	{
		perror("ERROR opening output file");
		return -1;
	}
	
	if(*argc[1] == 'c')
		huff_cod(in, out);
	else if(*argc[1] == 'd')
		huff_dec(in, out);
	else
		printf("Uso: %s [c/d] input output\n", argc[0]);
	
	fclose(in);
	fclose(out);
	return 0;
}