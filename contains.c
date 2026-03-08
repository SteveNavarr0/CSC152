//contains.c by Steve Navarro. Submitted for CSC 152 Februrary 8,2026.
//Compares two buffers and returns non zero if the second buffer is found in the first buffer sequentially
#include <stdio.h>
#include <stdint.h>

   int contains(void *a, int alen, void *b, int blen){
	unsigned char *A = a;
	unsigned char *B = b;
	
	if (blen == 0){ //handles empty buffer case
		return 1;
	} else for (int i = 0; i + blen <= alen; i++){ //loops through the a buffer
		for(int j = 0; j < blen; j++){ //loops through the b buffer
		   	if(A[i+j] != B[j])
				break;
		   	if(j+1 == blen){
 		    		return 1;
			}	 
		} //j for loop
	} //i for loop
	return 0;
   	} //contains

