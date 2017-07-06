//Leopoldo Melendez
#include<iostream>
#include<iomanip>
#include<cctype>
#include<cstdlib>
#include<ctime>
#include "Menu.h"
#include "LList.h"
#include "LStack.h"
#include "LQueue.h"
#include "LSQueue.h"
#include<time.h>

using namespace std;

template< class T >
void add( LStack<T>& stk, LQueue<T>& que, LSQueue<T> & sque,  int count ) {
    T v;
    for( int i = 0; i < count; i++ ) {
	v = rand() % 100;
	stk.push( v );
	que.enque( v );
	sque.enque( v );
    }
}

template< class T >
void input( LStack<T>& stk, LQueue<T>& que, LSQueue<T>& sque ) {
    T value;
    cout << "\t\tEnter a value : ";
    cin >> value;
    cin.ignore();
    stk.push( value );
    que.enque( value );
    sque.enque( value );
}

template< class T >
void show( LStack<T> stk, LQueue<T> & que, LSQueue<T>& sque ) {
    stk.show();
    que.show();
    sque.show();
}

template< class T >
void remove( LStack<T> & stk, LQueue<T> & que, LSQueue<T>& sque ) {
    stk.pop();
    que.deque();
    sque.deque(sque.frnt());
}

int main() {
    int num, pos;
    srand( (unsigned)(time(NULL) ) );
    LStack<int> stk;
    LQueue<int> que;
    LSQueue<int> sque;
    LStack<double> dStk;
    LQueue<double> dQue;
    LSQueue<double> dSque;


    const char * testMenu[] = {
	"+=========================================+",
	"|      Stack, Queue and SQueur Test       |",    
	"+=========================================+",
	"| F/f: Fill two sets of  Stack, Queues,   |",
	"|      and Sorted Queue with 10 integers  |",
	"|      and 10 double values               |",
	"+-----------------------------------------+",
	"| S/s: Show contents of 6 lists.          |",
	"+-----------------------------------------+",
	"| A/a: Ask for a double and an integer,   |",
	"|      into 6 structures.                 |",
	"+-----------------------------------------+",
	"| D/d: Remove one element from 6 list     |",
	"|      pop, deque function.               |",
	"+-----------------------------------------+",
	"| H/h/?: Show this menu.                  |",
	"+-----------------------------------------+",
	"| Q/q: Exit program.                      |",
	"+-----------------------------------------+",
	    NULL
    };

    char choice = ' ';

    Menu m( testMenu );


    while( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch( toupper(choice) ){
	    case ' ' : break;
	    case 'H' : m.show(); break;
	    case 'Q' : exit(0);
	    case 'F' : 
		       add( stk, que, sque, 10 );
		       add( dStk, dQue, dSque, 10 );
		       break;
	    case 'S' :
		      show( stk, que, sque );
		      show( dStk, dQue, dSque );
		       break;
	    case 'A' :
		       input( stk, que, sque );
		       input( dStk, dQue, dSque );
		       break;
	    case 'D' :
		       remove( stk, que, sque );
		       remove( dStk, dQue, dSque );
		       break;
	    default:
		       cout << "\t\tIllegal selection.\n";
	}
    }

    return 0;
}

