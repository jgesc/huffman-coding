#include "huff.h"

struct node
{
	unsigned int w; // Weight
	unsigned short p; // Parent
	unsigned short c0, c1; // 0 / 1
	unsigned char cl; // Code length
};

void huff_cod(FILE * in, FILE * out)
{
	// Iterators and general purpose variables
	int i, j;
	
	/// Build frequency table
	unsigned int ftable[256] = {0}; // Counts ocurrences per word
	unsigned char dirtab[256]; // Redirection table
	
	while(!feof(in)) // Iterate through stream
		ftable[fgetc(in)]++; // Increase counter for that character
	
	/// Build tree
	struct node tree[512000] = {0}; // Memory to store the tree
	unsigned short fnodes[256]; // List of free sub-trees
	unsigned short a, b; // a = smallest node; b = second smallest
	
	j = 0;
	for(i = 0; i < 256; i++) // Initialize leaves and free nodes
	{
		if(ftable[i])
		{
			dirtab[i] = j;
			fnodes[j] = j;
			tree[j].w = ftable[i];
			j++;
		}
	}
	unsigned short inode = j; // Next free node
	unsigned short lfnode = j - 1; // Last occupied position in fnodes

	while(lfnode > 0) // Iterate until the tree is complete
	{
		if(tree[fnodes[0]].w < tree[fnodes[1]].w) // Initialize a and b
		{
			a = 0;
			b = 1;
		}
		else
		{
			b = 0;
			a = 1;
		}
		
		for(i = 2; i <= lfnode; i++) // Find two smallest nodes
		{
			unsigned int w = tree[fnodes[i]].w;
			if(w < tree[fnodes[a]].w)
			{
				b = a;
				a = i;
			}
			else
			{
				if(w < tree[fnodes[b]].w) b = i;
			}
		}
		// Fill data
		unsigned short _a = fnodes[a];
		unsigned short _b = fnodes[b];
		tree[_a].p = inode;
		tree[_b].p = inode;
		tree[inode].c0 = tree[_a].cl <= tree[_b].cl ? _a : _b;
		tree[inode].c1 = tree[_a].cl > tree[_b].cl ? _a : _b;
		tree[inode].w = tree[_a].w + tree[_b].w;
		tree[inode].cl = tree[_a].cl + 1;
		
		fnodes[a] = inode++; // Replaces free node a with the newly created node
		fnodes[b] = fnodes[lfnode--]; // Replaces free node b with the last free node;
		
	} // End tree building
	
#ifdef DEBUG
	for(i = 0; i < inode; i++) printf("%d> p = %hu, c0 = %hu, c1 = %hu, w = %6u, cl = %uc\n", i, tree[i].p, tree[i].c0, tree[i].c1, tree[i].w, tree[i].cl);
#endif
	
	/// Encode stream
	unsigned char stack[256]; // Traversed nodes stack
	unsigned char stackp = 0; // Stack pointer
	size_t fs = ftell(in); // File size
	
	fwrite(&fs, sizeof(size_t), 1, out); // Write uncompressed file size
	fwrite(ftable, sizeof(unsigned int), 256, out); // Write frequency table
	rewind(in); // Back to the beginning
	
	unsigned char mask = 0x80;
	unsigned char byte = 0x00;
	while(1) // Encode all bytes
	{
		unsigned short inraw = fgetc(in);
		if(feof(in)) break;
		unsigned short curr = dirtab[inraw];
		
		while(tree[curr].p) // Iterate backwards through the tree
		{
			stack[stackp++] = (curr == tree[tree[curr].p].c1) ? 0xFF : 0x00; // Add bit to the pointer
			curr = tree[curr].p;
		}
		
		while(stackp)
		{
			byte |= mask & stack[--stackp];
			mask >>= 1;
			if(!mask)
			{
				mask = 0x80;
				fputc(byte, out);
				byte = 0x00;
			}
		}
	}
	if(mask != 0x80) fputc(byte, out);
}

void huff_dec(FILE * in, FILE * out)
{
	// Iterators and general purpose variables
	int i, j;
	size_t length;
	size_t rcount;
	
	/// Build frequency table
	unsigned int ftable[256] = {0}; // Counts ocurrences per word
	unsigned char dirtab[256]; // Inverse redirection table
	
	if(fread(&length, sizeof(size_t), 1, in) < 1)
	{
		printf("Can't decode file.\n");
		exit(-1);
	}
	if(fread(ftable, sizeof(unsigned int), 256, in) < 256) // Read frequency table
	{
		printf("Can't decode file.\n");
		exit(-1);
	}
	
	/// Build tree
	struct node tree[512000] = {0}; // Memory to store the tree
	unsigned short fnodes[256]; // List of free sub-trees
	unsigned short a, b; // a = smallest node; b = second smallest
	
	j = 0;
	for(i = 0; i < 256; i++) // Initialize leaves and free nodes
	{
		if(ftable[i])
		{
			dirtab[j] = i;
			fnodes[j] = j;
			tree[j].w = ftable[i];
			j++;
		}
	}
	unsigned short inode = j; // Next free node
	unsigned short lfnode = j - 1; // Last occupied position in fnodes

	while(lfnode > 0) // Iterate until the tree is complete
	{
		if(tree[fnodes[0]].w < tree[fnodes[1]].w) // Initialize a and b
		{
			a = 0;
			b = 1;
		}
		else
		{
			b = 0;
			a = 1;
		}
		
		for(i = 2; i <= lfnode; i++) // Find two smallest nodes
		{
			unsigned int w = tree[fnodes[i]].w;
			if(w < tree[fnodes[a]].w)
			{
				b = a;
				a = i;
			}
			else
			{
				if(w < tree[fnodes[b]].w) b = i;
			}
		}
		// Fill data
		unsigned short _a = fnodes[a];
		unsigned short _b = fnodes[b];
		tree[_a].p = inode;
		tree[_b].p = inode;
		tree[inode].c0 = tree[_a].cl <= tree[_b].cl ? _a : _b;
		tree[inode].c1 = tree[_a].cl > tree[_b].cl ? _a : _b;
		tree[inode].w = tree[_a].w + tree[_b].w;
		tree[inode].cl = tree[_a].cl + 1;
		
		fnodes[a] = inode++; // Replaces free node a with the newly created node
		fnodes[b] = fnodes[lfnode--]; // Replaces free node b with the last free node;
		
	} // End tree building
	
#ifdef DEBUG
	for(i = 0; i < inode; i++) printf("%d> p = %hu, c0 = %hu, c1 = %hu, w = %6u, cl = %uc\n", i, tree[i].p, tree[i].c0, tree[i].c1, tree[i].w, tree[i].cl);
#endif
	
	/// Decode stream
		
	unsigned char mask = 0x80;
	unsigned char byte = fgetc(in);
	for(i = 0; i < length; i++) // Decode to original size
	{
		if(feof(out))
		{
			printf("Can't decode file.\n");
			exit(-1);
		}
		unsigned short curr = inode - 1;
		while(tree[curr].cl) // Follow the branches
		{
			curr = byte & mask ? tree[curr].c1 : tree[curr].c0;
			mask >>= 1;
			if(!mask)
			{
				mask = 0x80;
				byte = fgetc(in);
			}
		}
		fputc(dirtab[curr], out);
	}
}