#include<iostream>
#include<iomanip>

using namespace std;

template< class T >
void iSort( T * a, int n ){
    T tmp;
    int i, j;
    for( i = 1; i < n; i++ ){
	tmp = a[i];
	
	for( j = i-1; j >= 0 && a[j] > tmp; j-- ) a[j + 1] = a[j];
	
	a[j + 1] = tmp;
    }
}

int main(){

    int array[10] = {3, 5, 2, 9, 4, 8, 6, 7, 12, 11};

    for( int i = 0; i < 10; i++ ){
	cout << array[i] << " ";
    }
    cout << endl;

    iSort(array, 10);

    for( int i = 0; i < 10; i++ ){
	cout << array[i] << " ";
    }
    cout << endl;
    return 0;
}
