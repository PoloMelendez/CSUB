#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include "hash.h"
#include<time.h>
#include<ctime>
#include "Menu.h"
using namespace std;

template< class T >
void generate( HashTable<T> ** table ){
    if( *table != NULL ){
	delete *table;
	*table = NULL;
    }
    int cap, count;
    float load;
    cout << "Enter the desired capacity : ";
    cin >> cap;
    cout << "Enter the desired Loading Factor : ";
    cin >> load;
    count = cap * load;
    int temp;

    *table = new HashTable<T>( cap, load, 0.2 );

    for( int i = 0; i < count; i++ ){
	temp = rand() % 100 + 1;
	(*table)->insert( temp );
    }

    cin.ignore();
}

template< class T >
void insert( HashTable<T> * table ){
    int value;
    cout << "Enter the value you wish to insert : ";
    cin >> value;
    table->insert( value );
    cin.ignore();
}

template< class T >
void remove( HashTable<T> * table ){
    int value;
    cout << "Enter the value you wish to remove : ";
    cin >> value;
    table->subtract( value );
    cin.ignore();
}

const char* testMenu[] = {
    "+==========================================+",
    "|           Binary Search Tree             |",
    "+==========================================+",
    "| G/g: Generate a randomly filled          |",
    "|      Hash Table.                         |",
    "+==========================================+",
    "| I/i: Insert an Integer into the tree.    |",
    "+==========================================+",
    "| R/r: Remove an Integer from the tree.    |",
    "+==========================================+",
    "| S/s: Show the contents of the            |",
    "|      Hash Table.                         |",
    "+==========================================+",
    "| H/h/?: Show this menu.                   |",
    "+==========================================+",
    "| Q/q: Exit program.                       |",
    "+==========================================+",
    NULL
};

int main() {
    srand(time(0));

    Menu m( testMenu );
    char v, choice = ' ';

    HashTable<Integer> * myHash = NULL;

    while ( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch (  toupper( choice)  ) {
	    case ' ' : break;
	    case 'H' : m.show(); break;
	    case 'Q' : exit(0);
	    case 'G' :
		       generate( &myHash );
		       break;
	    case 'S' : 
		       myHash->display();
		       break;
	    case 'I' :
		       insert( myHash );
		       break;
	    case 'R' : 
		       remove( myHash );
		       break;
	}
    }
    return 0;
}
