#include <stdio.h>
#include <time.h>
#include "demo3_typedef.h"
#include "demo3_sort.h"

typedef void (*Ptr_SortFunc)(int* arr,int len);

// declare functions:
static void print_array(int* arr, int len);
static void swap_array(int* a, int* b);
void bubble_sort(int* arr, int len);
void selection_sort(int* arr, int len);
void insertion_sort(int* arr, int len);
void shell_sort(int* arr, int len);
void merge_sort(int* arr, int len);
void heap_sort(int* arr, int len);
void quick_sort(int* arr, int len); 

Ptr_SortFunc _sort_func[] = { bubble_sort, selection_sort, insertion_sort, shell_sort, merge_sort, heap_sort, quick_sort };
#define DEBUG_PRINT_SORT_FUNC()\
do{\
    printf("LEVEL\t|\t bubble\t selection\t insertion\t shell\t merge\t heap\t quick\n");\
}while(0)

#define DEBUG_RUN_SORT_FUNC(FUNC,ARRAY,SIZE)\
do{\
   FUNC(ARRAY,SIZE);\
}while(0)

#define _PERFORMANCE_TEST

void demo3_simple_sort_test(){

    _DEBUG_ENTER(demo3_simple_sort_test);
    int m, n, i, N;
    int step = 1000;
    clock_t start,finish;
    int i_func = sizeof(_sort_func)/sizeof(Ptr_SortFunc);
    
#ifdef _PERFORMANCE_TEST
    //print function name tab:
    DEBUG_PRINT_SORT_FUNC();

    for(m=step; m<100000; m+=step) // 10w+
    {
        N = m;
        int a[N];

        printf("%d \t|\t",m);
        for(i=0; i<i_func; i++)
        {
            for(n=0; n<N; n++)
            {
                a[n] = rand()%1000;
            }

            start = clock();
            _sort_func[i](a,N);
            finish = clock();
            printf("%f\t",(finish-start)*1.0/CLOCKS_PER_SEC);
        }

        printf("\n");
    
    }
#endif

    //use static array[] for testing
#ifndef _PERFORMANCE_TEST
    static int array[8] = { 8, 1, 3, 5, 6, 4, 2, 7 };
    DEBUG_RUN_SORT_FUNC(insertion_sort,array,8);
    print_array(array,8);
#endif

    _DEBUG_LEAVE(demo3_simple_sort_test);
}

/* Swap sort : bubble, quick ; 
 * Bubble sort, sometimes incorrectly referred to as sinking sort, is a simple sorting algorithm that works by repeatedly stepping through the list to be sorted, comparing each pair of adjacent items and swapping them if they are in the wrong order. 
 * The pass through the list is repeated until no swaps are needed, which indicates that the list is sorted. The algorithm gets its name from the way smaller elements "bubble" to the top of the list. 
 * Because it only uses comparisons to operate on elements, it is a comparison sort. Although the algorithm is simple, most of the other sorting algorithms are more efficient for large lists.
 */
void bubble_sort(int* a, int size) /* worst: O(n^2) */
{
    //_DEBUG_ENTER(bubble_sort);

    int i,j;
    for(i=1; i<size-1; i++)
        for(j=0; j<size-i; j++)
        {
            if(a[j]>a[j+1])
            {
                swap_array(&a[j],&a[j+1]);
            }
        }
}

/* 
 * 快速排序
 * 时间复杂度：O（n*lgn）
 * 最坏：O（n^2）
 * 空间复杂度：O（n*lgn）
 */
void quick_sort(int *a, int size)
{
    //_DEBUG_ENTER(quick_sort);

    int i,j;

}


/* Selection sort: 选择, 堆排序
 * Selection sort: The algorithm divides the input list into two parts: the sublist of items already sorted, which is built up from left to right at the front (left) of the list, and the sublist of items remaining to be sorted that occupy the rest of the list. 
 * Initially, the sorted sublist is empty and the unsorted sublist is the entire input list. 
 * The algorithm proceeds by finding the smallest (or largest, depending on sorting order) element in the unsorted sublist, exchanging it with the leftmost unsorted element (putting it in sorted order), and moving the sublist boundaries one element to the right.
 * */
void selection_sort(int *a, int size) /* worst: O(n^2) */
{
    //_DEBUG_ENTER(selection_sort);

    int i,j,max;
    for(i=0; i<size-1; i++)
    {
        max = 0;
        for(j=0; j<size-i; j++)
        {
            if(a[j]>a[max])
                max = j;
        }
        if(max != (size-i-1))
            swap_array(&a[max], &a[size-i-1]);
    }
}

void heap_sort(int* a, int size)
{
}


// Insertion sort： 插入排序, 二分排序
// (1) 插入排序
void insertion_sort(int *a, int size) /* worst: O(n^2) */
{
    int i,j;
    int tmp;

    for(i=1; i<size; i++)
    {
        tmp = a[i];
        for(j=i-1; j>=0; j--)
        {
            if(a[j]>tmp){
                a[j+1] = a[j];
            }
            else{
                // a[j+1]=tmp;
                break;
            }
        }
        a[j+1]=tmp;
    }
}

// (2) 二分排序
void binary_sort(int* a, int size)
{
}
void shell_sort(int* a, int size)
{
}

void merge_sort(int* arr, int len)
{
}




static void print_array(int* arr, int len)
{
    int i=0;
    while(i<len){
        printf("%d\t",arr[i++]);
    }
    printf("\n");

}
static void swap_array(int *a,int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

