#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>

void quick_sort(int arr[], int len);
void printl(int arr[],int size);
void merge_l(int *a,int *b,int *temp,int size1,int size2);

int main()
{
	int comm_size_tmp;
    int my_id;
    int arr_size = 0;

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size_tmp);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    const int comm_size = comm_size_tmp;
	
	if(my_id == 0)
	{
		printf("input array size:\n");
		scanf("%d",&arr_size);
	}
	MPI_Bcast(&arr_size, 1, MPI_INT , 0, MPI_COMM_WORLD);

	//build gatherv maps
    int displs[comm_size];
    int scounts[comm_size];
	int remainder = arr_size % comm_size;
    for (int i=0; i<comm_size; ++i) {
		scounts[i] = arr_size/comm_size + (remainder-- > 0);
		displs[i] =  (i==0) ? 0 : (displs[i-1]+scounts[i-1]);
    }
	if(my_id == 0)
	{
		for (int i=0; i<comm_size; ++i) {
        	printf("i:%d count:%d displ:%d\n",i,scounts[i] ,displs[i]);
    	}
	}
	int total_size = arr_size;
	arr_size = scounts[my_id];

	int arr[arr_size];

	srand(time(NULL) ^ getpid());
	for(int i=0;i <arr_size;i++)
	{
		arr[i] = rand()%1000;
	}

	quick_sort(arr,(int) sizeof(arr)/sizeof(arr[0]));
	printf("process %d get :",my_id);
	printl(arr,arr_size);

	int arr2[arr_size+1];
	int temp[2*arr_size+2];
	int my_front = my_id -1;
	int my_back = (my_id +1 >= comm_size) ?-1 : my_id+1;

	//counting how many times to merge
	int count = comm_size -1 + comm_size %2 ;
	while(count-- >0)
	{
		if( (!(my_id %2))&& my_back >=0)
		{
			MPI_Recv(arr2, scounts[my_back], MPI_INT, my_back, 1,
            	    MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
			merge_l(arr,arr2,temp,arr_size,scounts[my_back]);
			memcpy(arr,temp,4*arr_size);
			MPI_Send(&(temp[arr_size]),scounts[my_back],MPI_INT, 
					my_back, 0, MPI_COMM_WORLD);
		}
		else if((my_id %2) && my_front >=0)
		{
			MPI_Send(arr,arr_size,MPI_INT, my_front, 1, MPI_COMM_WORLD);
			MPI_Recv(arr, arr_size, MPI_INT, my_front, 0,
                	MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
		if(count-- >0)
		{
			//printf("%d inside  count:%d id2:%d \n",my_id,count,my_id%2);
			if( (my_id %2)&& my_back >=0)
        	{
            	MPI_Recv(arr2, scounts[my_back], MPI_INT, my_back, 1,
                    	MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            	merge_l(arr,arr2,temp,arr_size,scounts[my_back]);
				memcpy(arr,temp,4*arr_size);
            	MPI_Send(&(temp[arr_size]),scounts[my_back],MPI_INT, my_back, 0, MPI_COMM_WORLD);
        	}
        	else if((!(my_id%2)) && my_front >=0)
        	{
            	MPI_Send(arr,arr_size,MPI_INT, my_front, 1, MPI_COMM_WORLD);
            	MPI_Recv(arr, arr_size, MPI_INT, my_front, 0,
            	        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        	}
		}
	}

	
	if(my_id ==0)
	{
		int total_arr[total_size];
		MPI_Gatherv( arr, arr_size, MPI_INT
            , total_arr, scounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
		printf("global sorted list:\n");
		printl(total_arr,total_size);
	}    
	else
	{
		MPI_Gatherv( arr, arr_size, MPI_INT
            ,NULL, scounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
	}	
	MPI_Finalize();
	return 0;
}

//merge two array into temp array in order
void merge_l(int *a,int *b,int *temp,int size1,int size2)
{
    int a_count = size1;
    int b_count = size2;
    while(a_count && b_count)
    {
        if(*a < *b)
        {
            *(temp++) =  *(a++);
            --a_count ;
        }
        else{
            *(temp++) =  *(b++);
            --b_count ;
        }
        //printf("%d ",*(temp-1));
    }
    while(a_count--)
        *(temp++) = *(a++);
    while(b_count--)
        *(temp++) = *(b++);
}

//print a array of size
void printl(int arr[],int size)
{
	int i=0;
	while(i < size)
	{
		printf("%d ",arr[i++]);
	}
	printf("\n");
}

//swap two ptr's data
void swap(int *x, int *y) 
{
	int t = *x;
	*x = *y;
	*y = t;
}

//do quicksort
void quick_sort_recursive(int arr[], int start, int end)
{
	if (start >= end)
		return;//這是為了防止宣告堆疊陣列時當機
	int mid = arr[end];
	int left = start, right = end - 1;
	while (left < right) {
		while (arr[left] < mid && left < right)
			left++;
		while (arr[right] >= mid && left < right)
			right--;
		swap(&arr[left], &arr[right]);
	}
	if (arr[left] >= arr[end])
		swap(&arr[left], &arr[end]);
	else
		left++;
    if (left) {
	    quick_sort_recursive(arr, start, left - 1);
	    quick_sort_recursive(arr, left + 1, end);
    } else {
        quick_sort_recursive(arr, left + 1, end);
    }
}

void quick_sort(int arr[], int len)
{
	quick_sort_recursive(arr, 0, len - 1);
}
