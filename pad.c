#include <stdio.h>
#include <stdlib.h>

#define HLEN		32
#define MAGIC		0x1EB
#define ESWAP(x)	(x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24)

#define PAGE_SHIFT		12
#define PAGE_SIZE		(1UL << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))
#define	PAGE_ALIGN(x)	(((x)+PAGE_SIZE-1)&PAGE_MASK)

FILE* fd = NULL;
FILE* fo = NULL;

struct plan9_exec {
	unsigned long magic;					
	unsigned long text;						
	unsigned long data;						
	unsigned long bss;						
	unsigned long syms;	 					
	unsigned long entry;					
	unsigned long spsz;						
	unsigned long pcsz;						
};

void endian_swap(struct plan9_exec *ex)
{
	ex->magic 	= ESWAP(ex->magic);
	ex->text 	= ESWAP(ex->text);
	ex->data 	= ESWAP(ex->data);
	ex->bss 	= ESWAP(ex->bss);
	ex->syms 	= ESWAP(ex->syms);
	ex->entry 	= ESWAP(ex->entry);
	ex->spsz 	= ESWAP(ex->spsz);
	ex->pcsz	= ESWAP(ex->pcsz);
}

void usage(const char* msg)
{
	if (!msg)
		printf("./pad <8.out> <output>\n");
	else
		printf("%s\n", msg);
	if (fd)
		fclose(fd);
	exit(0);
}

int main(int argc, char* argv[])
{
	int pd;
	char* bss;
	char* pad;
	char* text;
	char* data;
	char header[HLEN];
	struct plan9_exec ex;
	
	if (argc != 3)
		usage(NULL);
		
	fd = fopen(argv[1], "rb");
	if (!fd)
		usage("Invalid input file");
		
	fread(header, sizeof(char), HLEN, fd);
	ex = *((struct plan9_exec *) header);
	endian_swap(&ex);
	
	printf("P9: %lx %lx %lx %lx %lx\n", ex.magic, ex.text, ex.data, ex.bss, ex.entry);
	
	if (ex.magic != MAGIC)
		usage("Invalid a.out file!\n");
	
	text = calloc(ex.text, sizeof(char));
	fread(text, sizeof(char), ex.text, fd);
	pd = PAGE_ALIGN(ex.text + HLEN) - (ex.text + HLEN);
	
	printf("P9: Padding %lx bytes from %lx\n", ex.text + HLEN);
	pad = calloc(pd, sizeof(char));
	
	data = calloc(ex.text, sizeof(char));
	fread(data, sizeof(char), ex.data, fd);
	
	bss = calloc(ex.bss, sizeof(char));
	fclose(fd);
	fd = NULL;
	fd = fopen(argv[2], "wb+");
	if (!fd)
		usage("Can't write to output file, Invalid!\n");
		
	fwrite(header, sizeof(char), HLEN, fd);
	fwrite(text, sizeof(char), ex.text, fd);
	fwrite(pad, sizeof(char), pd, fd);
	fwrite(data, sizeof(char), ex.data, fd);
	fwrite(bss, sizeof(char), ex.bss, fd);
	printf("Done! Output written to %s\n", DESTINATION);
	
	fclose(fd);
}