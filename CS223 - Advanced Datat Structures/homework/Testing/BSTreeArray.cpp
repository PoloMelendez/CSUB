#ifndef HEAP_H
#define HEAP_H
#include<iostream>
#include<iomanip>

using namespace std;

template< class T >
class Heap{
    private:
	T * arr;
	int n, cap;

    public:
	Heap( int cap = 10 ) { 
	    this->cap = cap; n = 0; arr = new T[cap];
	    cout << "Constructor Called: Cap: " << this->cap << endl;
	    for( int i = 0; i < this->cap; i++ )
		arr[i] = 0;
       	}

	void insert( T element ){
	    int curr = 0;
	    while(true){
		if(arr[curr] == 0){
		    arr[curr] = element;
		    break;
		}
		else if( arr[curr] < element )
		    curr = (curr*2 + 2);
		else if( arr[curr] >= element )
		    curr = (curr*2 + 1);
	    }
	    n++;
	}

	int findLargerChild( int r, int n ) {
	    int leftChild = r * 2 + 1, rightChild = leftChild + 1;
	    if( rightChild > n ) return leftChild;
	    return arr[rightChild] >= arr[leftChild] ? rightChild : leftChild;
	}

	void perculateDown( int r, int n ) {
	    if( r >= n ) return;
	    int k = findLargerChild( r );
	    if( k < 0 ) return;
	    if(arr[k] > arr[r]) swap( arr[r], arr[k] );
	    perculateDown( k, n );
	}

	void display(){
	    for(int i = 0; i < cap; i++)
		cout << arr[i] << " ";
	    cout << endl;
	    cout << "Cap: " << cap << "  Count: " << n << endl;
	}
};

template< class T >
class PriorityQueue : public Heap<T> {
};

template< class T >
class HeapSort : public Heap<T> {
};
#endif
