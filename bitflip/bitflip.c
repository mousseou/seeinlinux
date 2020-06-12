//Matias Lago, Problem1 of assignment 1
#include <stdio.h>
#include <stdlib.h>
#define WSIZE 32 

unsigned int ipow(unsigned int base, unsigned int exp) {
	unsigned n = 1;
	while(exp > 0) {
		n *= base;
		exp--;
	}
	return n;
}

void print_bin(char *num, int len) {
	for(int i = 0; i < len; i++) {
		printf("%c", num[i]);
		if((i+1) % 4 == 0)
			printf(" ");
	}
	printf("\n");
}

unsigned int translate_bin(char *bin, int len) {
	unsigned int n = 0;
	for(int i=0; i < WSIZE-1; i++) {
		n += (bin[i] == '1') ? ipow(2, WSIZE-1-i) : 0;
	}	
	return n;
}

int main(int argc, char **argv) {
	unsigned int original = atoi(argv[1]);
	
	char o[WSIZE];
	char i[WSIZE];
	for(int w=WSIZE-1; w >= 0; w--) {
		char bit = ((original >> (WSIZE-1-w)) & 0x1) ? '1' : '0';
		o[w] = bit;
		i[WSIZE-1-w] = bit;
	}	
	printf("%d:\t", original);
	print_bin(o, WSIZE);	
	printf("%u:\t", translate_bin(i, WSIZE));
	print_bin(i, WSIZE);
	return 0;
}
