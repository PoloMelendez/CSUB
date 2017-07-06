#include<iostream>
#include "TStack.h"

using namespace std;

int main() {

    TStack<int> * ptr = new TStack<int> ();

    for( int i = 0; i < 10; i ++)
	ptr->push( i * 10 );

    delete ptr;


    return 0;    
}
