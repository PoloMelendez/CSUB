//Heap
//Polo Melendez
//cs312
#include<iostream>

using namespace std;

void heapify(int *, int *, int);
void perculateDown(int *, int);
void heapSort(int *);
void display(int *, int);

int main(int argc, char ** argv) {

    int arr[100] = { 5,3,8,12,9,4,10 };
    int n = 7;
    int heap[100];

    heapify(arr, heap, n);

    display(heap, n+1);
    heapSort(heap);
    display(heap, n+1);

    return 0;
}

void heapSort(int *heap) {
    int n = heap[0];
    while(heap[0] > 0) {
        int tmp = heap[heap[0]];
        heap[heap[0]] = heap[1];
        heap[1] = tmp;
        heap[0]--;
        perculateDown(heap, 1);
    }
    heap[0] = n;
}

void display(int *arr, int n) {
    for(int i=0; i<n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void perculateDown(int *heap, int parent) {
    while(parent <= heap[0]/2) {
        int maxChild, leftChild, rightChild;
        leftChild = parent * 2;
        rightChild = leftChild + 1;
        maxChild = leftChild;
        if(rightChild <= heap[0]) {
            if(heap[rightChild] > heap[leftChild]) {
                maxChild = rightChild;
            }
        }
        if(heap[parent] < heap[maxChild]) {
            int tmp = heap[parent];
            heap[parent] = heap[maxChild];
            heap[maxChild] = tmp;
            parent = maxChild;
        }
        else {
            break;
        }
        parent = maxChild;
    }
}

void heapify(int *arr, int *heap, int n) {
    for(int i=0; i<n; i++) {        //copy arr into heap, leaving heap[0] for n
        heap[i+1] = arr[i];
    }
    heap[0] = n;
    int parent = n/2;
    while(parent >= 1) {
        perculateDown(heap, parent);
        parent--;
    }
}
