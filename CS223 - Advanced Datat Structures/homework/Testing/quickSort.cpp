#include<iostream>
#include<iomanip>
#include<ctime>
#include<cstdlib>

using namespace std;

template< class T >
int split( T* arr, int start, int end ){
    //int randomIndex = rand()%(end - start + 1) + start;
    int randomIndex = arr[end];
    cout << randomIndex << endl;
    if( start < end ) swap( arr[start], arr[randomIndex] );
    int i = start + 1, j = end;
    while( i <= j ){
	while( i <= end && arr[i] <= arr[start] ) i++;
	while( j >= start && arr[j] >= arr[start] ) j--;
	if( i < j ) swap( arr[i], arr[j] );
    }
    if( start < j ) swap( arr[start], arr[j] );
    return j;
}

template< class T >
void qSort( T* arr, int start, int end ){
    if( start >= end ) return;
    int middle = split( arr, start, end);
    qSort( arr, start, middle-1 );
    qSort( arr, middle+1, end );
}

template< class T >
void qSort( T* arr, int size ){
    qSort( arr, 0, size-1 );
}

template< class T >
void fill( T * arr, int cap ){
    for( int i = 0; i < cap; i++){
	arr[i] = rand()%100 + 1;
    }
}

template< class T >
void display( T* arr, int cap ){
    for( int i = 0; i < cap; i++){
	cout << arr[i] << " ";
    }
    cout << endl;
}

int main(){
    srand(time(0));
    int arr[25];

    fill(arr, 25);
    display( arr, 25 );

    qSort( arr, 25 );

    display( arr, 25 );


    return 0;
}
