#include <stdlib.h>
#include <iostream>

void swap(int* a,int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void quickSort(int arr[],int start, int end){
    if (start >= end) return;

    //pivot
    int pivot = arr[start];
    
    // Set left and right pointer 
    int l = start + 1;
    int r = end;

    //partion items 
    while (l <= r){
        // move left pointer till we find a value greater than the pivot 
        while ( l <= r && arr[l] <= pivot){
            l++;
        }

        //move right pointer till we find a value less than the pivot 
        while (l <= r && arr[r] > pivot){
            r--;
        }

        if (l <= r){
            swap(&arr[l++],&arr[r--]);
        }

    }

    swap(&arr[start],&arr[r]);

    quickSort(arr,r+1,end);
    quickSort(arr,start,r-1);
    
}

int main(){

    int arr[] = {-12,-11,-1,2,-22,12};
    quickSort(arr,0,5);
    
    return 1;
};