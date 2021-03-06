#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define THREAD_COUNT 4

void Count_sort (int a [], int n ) 
{

	int i , j , count ;
	int* temp = malloc ( n* sizeof(int));

	#pragma omp parallel for  num_threads(THREAD_COUNT)  private(i,j,count)	
	for ( i = 0; i < n ; i ++) {
		count = 0;
		for ( j = 0; j < n ; j ++)
			if ( a [j] < a [i])
				count ++;
			else if ( a [ j ] == a [ i ] && j < i )
				count ++;
		temp [ count ] = a [ i ];
	}


	memcpy ( a , temp , n*sizeof(int));
    free ( temp );

}
/* Count sort */
