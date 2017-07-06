#ifndef HEAP_H
#define HEAP_H
#include<iostream>
#include<iomanip>
#include<ctime>
#include<cstdlib>
#include<cassert>

using namespace std;

template< class T >
class Heap{
    private:
        T * arr;
        int n, cap;

    public:
        Heap( int cap = 10 ) { 
            this->cap = cap; n = 0; arr = new T[cap];
            for( int i = 0; i < this->cap; i++ )
                arr[i] = 0;
        }

        ~Heap() {
            if( arr != NULL ) {
                delete[] arr;
                arr = NULL;
            }
        }

        void insert( T element ){
            arr[n] = element;
            perculateUp( 0, n );
            n++;
        }

        void remove( T element ){
            int index = -1;
            for(int i = 0; i < n; i++ ) {
                if( arr[i] == element )
                    index = i;
            }
            if( index == -1 ) {
                cout << "Item not in the heap!" << endl;
                return;
            }
            else {
                swap(arr[index], arr[n-1]);
                arr[n-1] = 0;
                n--;
                perculateDown( 0, n - 1 );
            }
        }

        void heapSort( T a[], int size ) {
            T * tempArr = arr;
            int count = n, capacity = cap;
            arr = a; n = size;
            heapify( 0, size-1 );
            for( int k = size -1; k > 0; k-- ){
                swap(a[0], a[k] );
                perculateDown( 0, k-1 );
            }
            arr = tempArr;
            n = count;
            cap = capacity;
        }

        int findLargerChild( int r, int n ) {
            int leftChild = r * 2 + 1, rightChild = leftChild + 1;
            if( rightChild > n ) return leftChild;
            return arr[rightChild] >= arr[leftChild] ? rightChild : leftChild;
        }

        void perculateDown( int r, int n ){
            int maxChild, rightChild, leftChild;
            T tmp;
            leftChild = r*2 + 1;
            rightChild = r*2 + 2;
            if(leftChild <= n) {
                if(leftChild == n) {
                    maxChild = leftChild;
                }
                else{
                    if(arr[leftChild] <= arr[rightChild])
                        maxChild = rightChild;
                    else
                        maxChild = leftChild;
                }
                if( arr[r] < arr[maxChild]) {
                    tmp = arr[r];
                    arr[r] = arr[maxChild];
                    arr[maxChild] = tmp;
                    perculateDown(maxChild, n);
                }
            }
        }
        /*void perculateDown( int r, int n ) {
          if( r >= n ) return;
          int k = findLargerChild( r, n );
          if( k < 0 ) return;
          if(arr[k] > arr[r]) swap( arr[r], arr[k] );
          perculateDown( k, n );
          }*/

        void perculateUp( int r, int n ) {
            int parent;
            T tmp;
            if( n > r ) {
                parent = ( n - 1 ) / 2;
                if( arr[parent] < arr[n] ) {
                    tmp = arr[parent];
                    arr[parent] = arr[n];
                    arr[n] = tmp;
                    perculateUp( r, parent );
                }
            }
        }

        void heapify( int r, int size ) {
            int middle = ( size - r -1 ) / 2;
            while( middle >= 0 ){
                perculateDown( middle, size );
                --middle;
            }
        }


        void display(){
            for(int i = 0; i < n; i++)
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
