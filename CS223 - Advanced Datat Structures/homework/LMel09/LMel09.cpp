#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include "heap.h"
#include<time.h>
#include<ctime>
#include "menu.h"
using namespace std;

void generate( int ** a, int &count ){
    if( *a != NULL ){
	delete *a;
	*a = NULL;
    }
    cout << "Enter the desired size : ";
    cin >> count;
    int temp;
    *a = new int[count];

    for( int i = 0; i < count; i++ ){
	temp = rand() % 1000 + 1;
	(*a)[i] = temp;
    }
    cin.ignore();
    cout << endl;
}

void show( int * a, int count ){
    char s[11];
    for( int i = 1; i < count+1; i++ ){
	cout << setw(8) << a[i-1];
	if( i % 10 == 0 && i+1 != count+1 ){ cout << endl;
	//if( i % 200 == 0 ) {
	    cout << "\t\tEnter 'Q' to stop ... ";
	    cin.getline(s, 11);
	    if( s[0] == 'Q' || s[0] == 'q') break;
	}
    }
	cout << endl << endl;
}

template< class T >
void insertionSort( T * a, int n ){
    int tmp;
    int i, j;
    for( i = 1; i < n; i++ ){
	tmp = a[i];

	for( j = i-1; j >= 0 && a[j] > tmp; j-- ) a[j + 1] = a[j];
	a[j + 1] = tmp;
    }
}

template< class T >
int split( T* arr, int start, int end ){
    int randomIndex = rand()%(end - start + 1) + start;
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
void quickSort( T* arr, int start, int end ){
    if( start >= end ) return;
    int middle = split( arr, start, end);
    quickSort( arr, start, middle-1 );
    quickSort( arr, middle+1, end );
}

template< class T >
void quickSort( T* arr, int size ){
    quickSort( arr, 0, size-1 );
}

void timingHeap( int a[], Heap<int> heap, int size ) {
    clock_t start, end;
    start = clock();
    heap.heapSort( a, size );
    end = clock();
    cout << "Heap sort took " << (float)(end - start)/CLOCKS_PER_SEC*0.001 << " MS or " <<
	(float)(end - start)/CLOCKS_PER_SEC << " seconds to sort " << size <<
	" integers.\n\n";
}

void timingInsertion( int a[], int size ) {
    clock_t start, end;
    start = clock();
    insertionSort( a, size );
    end = clock();
    cout << "Insertion sort took " << (float)(end - start)/CLOCKS_PER_SEC*0.001 << " MS or " 
	<< (float)(end - start)/CLOCKS_PER_SEC << " seconds to sort " << size <<
	" integers.\n\n";
}

void timingQuick( int a[], int size ) {
    clock_t start, end;
    start = clock();
    quickSort( a, size );
    end = clock();
    cout << "Quick sort took " << (float)(end - start)/CLOCKS_PER_SEC*0.001 << " MS or " <<
	(float)(end - start)/CLOCKS_PER_SEC << " seconds to sort " << size <<
	" integers.\n\n";
}

void shuffle( int a[], int size ){
    for( int i =0; i < size; i++ )
	swap( a[i], a[rand()% size] );
    cout << "\t\tArray integers shuffled.\n";
}



const char* testMenu[] = {
    "+===========================================+",
    "|           Sorting Alogrithmns             |",
    "+===========================================+",
    "| G/g: Generate a randomly filled           |",
    "|      Array of integers.                   |",
    "+===========================================+",
    "| S/s: Show the contents of the array.      |",
    "+===========================================+",
    "| 1: Call Insertion Sort to sort the array. |",
    "+===========================================+",
    "| 2: Call Quick Sort to sort the array.     |",
    "+===========================================+",
    "| 3: Call Heap Sort to sort the array.      |",
    "+===========================================+",
    "| 0: Shuffle the integers in the array.     |",
    "+===========================================+",
    "| H/h/?: Show this menu.                    |",
    "+===========================================+",
    "| Q/q: Exit program.                        |",
    "+===========================================+",
    NULL
};

int main() {
    srand(time(0));

    Menu m( testMenu );
    char v, choice = ' ';

    Heap<int> myHeap;
    int count;
    int * array = NULL;

    while ( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch (  toupper( choice)  ) {
	    case ' ' : break;
	    case 'H' : m.show(); break;
	    case 'Q' : exit(0);
	    case 'G' :
		       generate( &array, count );
		       break;
	    case 'S' : 
		       show( array, count );
		       break;
	    case '1' :
		       timingInsertion( array, count );
		       break;
	    case '2' : 
		       timingQuick( array, count );
		       break;
	    case '3' :
		       timingHeap( array, myHeap, count );
		       break;
	    case '0' :
		       shuffle( array, count );
		       break;
	}
    }
    return 0;
}
