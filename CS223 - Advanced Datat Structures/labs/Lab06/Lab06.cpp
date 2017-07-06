#include<iostream>
#include<ctime>
#include<time.h>
#include<iomanip>
#include<cstdlib>

using namespace std;

const int MAXINT = 100000;

void arrayFill( int * arr ) {
    for(int i = 0; i < 1001;i++)
	arr[i] = rand() % MAXINT; 
}

void arraySearch( int * arr, int num ){
    bool found = false;
    int index = 0;
    while(index!=1001 && !found) {
	if(arr[index] == num )
	    found = true;
	else
	    index++;
    }
    if( found )
	cout<<"Found input: "<<num<<" at "<<index<< endl;
}

int main() {
    srand(time(NULL));

    int array[1001];

    arrayFill( array );

    for(int i = 0; i < 1001; i++)
	arraySearch( array, rand() % MAXINT );

    return 0;
}
