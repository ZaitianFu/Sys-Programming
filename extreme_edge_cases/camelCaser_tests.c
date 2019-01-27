/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.i
char* ab="I waNna  .Sle(E)p/; ORcs 241 .  I dK!!!12  3a!!!! no n";
char* ac=NULL;
//char* ac=" aBc. abc// ab8c@ abc Abc.";
//char* a=malloc(strlen(ab)*sizeof(char)+1);
//strcpy(a,ab);
char** b=(*camelCaser)(ab);
char** c=(*camelCaser)(ac);
for (int i=0; i<13;i++)
	printf("%s \n",b[i]);

char* sol[14];
sol[0]="iWanna";
sol[1]="sle";
sol[2]="e";
sol[3]="p" ;
sol[4]="\0";
sol[5]="orcs241";
sol[6]="iDk";
sol[7]="\0";
sol[8]="\0";
sol[9]="123A";
sol[10]="\0";
sol[11]="\0";
sol[12]="\0";
sol[13]=NULL;
if (c !=NULL) return 0;
int row=0;
for (int i=0;b[i]!=NULL;i++)
	row++;
if (row!=13) return 0;
printf("%d\n",row);
for (int i=0; i<13;i++){
       if (strcmp(b[i],sol[i])!=0){
	//	free(sol);
		destroy(b);
		return 0;}
}
//free(sol);
destroy(b);

    return 1;
}
