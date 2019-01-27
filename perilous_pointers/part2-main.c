/**
 * Pointers Gone Wild Lab
 * CS 241 - Spring 2018
 */

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// partner: xinyey2 zhao108 
/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
	// your code here
	first_step(81);

	int a=132; 
	int* step2=&a;
	second_step(step2);

	a=8942;
	int* tmp=&a;
	int** tmp2=&tmp;
	double_step(tmp2);

	char*b=(char*)calloc(10, 1);
	b[5]=(char)15;
	strange_step(b);
	free(b);b=NULL;

	void* c=(void*)calloc(10,1);
	*((char *)(c+3)) = (char)0;
	empty_step(c);
	free(c);c=NULL;

	char* s2="uuuuuu";
	void* s=s2;
	two_step(s,s2);

        
	char* a1="12345";
	char* a2="12345";
	char* a3="12345";	
	a2=a1+2;
	a3=a2+2;
	three_step(a1, a2,a3);
	
	char*b0=(char*)calloc(10,1);
	char*b1=(char*)calloc(10,1);
	char*b2=(char*)calloc(10,1);
	b0[1]=(char)0;
	b1[2]=(char)8;
	b2[3]=(char)16;
	step_step_step(b0, b1, b2);
	free(b0);free(b1);free(b2);

	char c1='9';
	char* c0=&c1;
	it_may_be_odd(c0,(int) c1);

	char d0[10]="I,CS241,k";
	char* d1=d0;
	tok_step(d1);
 
	void *blue=(void*)calloc(10,1);
	//*(char*)blue=(char)1;
	void* orange=blue;
	*((int *)orange)=0x1101;
	the_end(orange, blue);
	free(blue);
    return 0;
}
