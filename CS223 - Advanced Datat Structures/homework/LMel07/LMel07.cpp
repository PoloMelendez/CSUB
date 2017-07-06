#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include "BSTree.h"
#include<time.h>
#include<ctime>
#include "menu.h"
using namespace std;

template<class T>
void fill( BSTree<T> & tree ) {
    for( int i = 0; i < 10; i++ )
	tree.insert( (char)(rand()%26) + 'A' );
    cout << endl;
}

template<class T>
void insertion( BSTree<T> & tree ) {
    char add;
    cout << "\tEnter the char to insert : ";
    cin >> add;
    add = toupper( add );
    tree.insert(add);
    cout << endl;
    cin.ignore();
}

template<class T>
void removal( BSTree<T> & tree ) {
    char rem;
    cout << "\tEnter the char to remove : ";
    cin >> rem;
    rem = toupper( rem );
    tree.erase(rem);
    cout << endl;
    cin.ignore();
}

template<class T>
void searching( BSTree<T> & tree ) {
    char srch;
    bool found;
    cout << "\tEnter the char to search for : ";
    cin >> srch;
    srch = toupper( srch );
    found = tree.search( srch );
    if( found ) cout << "\tCharacter " << srch << " is in the tree.\n\n";
    else cout << "\tCharacter " << srch << " is not in the tree.\n\n ";
    cin.ignore();
}

template<class T>
void createCopy( BSTree<T> & tree ) {
    BSTree<char> copy( tree );
    copy.print();
    cout << "\t\t\t***This is the copied Tree***\n";
}    

const char* testMenu[] = {
    "+==========================================+",
    "|           Binary Search Tree             |",
    "+==========================================+",
    "| G/g: Generate a BST with 10 char Nodes.  |",
    "+==========================================+",
    "| C/c: Test the copy constructor           |",
    "+==========================================+",
    "| F/f: Search the tree for a given char.   |",
    "+==========================================+",
    "| I/i: Insert a character into the tree.   |",
    "+==========================================+",
    "| R/r: Remove a character from the tree.   |",
    "+==========================================+",
    "| P/p: Print the BST in tree shape.        |",
    "+==========================================+",
    "| S/s: Show the longest and shortest       |",
    "|      paths and the size.                 |",
    "+==========================================+",
    "| T/t: Show the BST in preorder, inorder,  |",
    "|      postorder, and RVL order.           |",
    "+==========================================+",
    "| H/h/?: Show this menu.                   |",
    "+==========================================+",
    "| Q/q: Exit program.                       |",
    "+==========================================+",
    NULL
};

int main() {
    srand(time(0));
    BSTree<char> ctree;

    Menu m( testMenu );
    char v, choice = ' ';

    while ( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch (  toupper( choice)  ) {
	    case ' ' : break;
	    case 'H' : m.show(); break;
	    case 'Q' : exit(0);
	    case 'G' :
		       ctree.clear();
		       fill( ctree );
		       break;
	    case 'C' :
		       createCopy( ctree );
		       break;
	    case 'F' :
		       searching( ctree );
		       break;
	    case 'I' :
		       insertion( ctree );
		       break;
	    case 'R' : 
		       removal( ctree );
		       break;
	    case 'P' : 
		       ctree.print();
		       break;
	    case 'S' : 
		       cout << "\tLongest Path : ";
		       cout << ctree.height() << endl;
		       cout << "\tShortest Path : ";
		       cout << ctree.shortHeight() << endl;
		       cout << "\tSize of BST : " << ctree.sizeofTree();
		       cout << endl << endl;
		       break;
	    case 'T' :
		       ctree.printOrder();
		       cout << endl;
		       break;
	}
    }
    return 0;
}
