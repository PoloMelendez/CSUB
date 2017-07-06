//Leopoldo Melendez
//
#include<iostream>
#include<iomanip>
#include<cctype>
#include<cstdlib>
#include<ctime>
#include "Menu.h"
#include "DFList.h"

using namespace std;

int main() {

    float value, index;

    const char * testMenu[] = {
	"C:  Create one floating number list,\n\t\tand insert 10 random float values in\n\t\tthe list. The value ranges of lists\n\t\t0.00 -- 4.00.",
	"S:  Show values in the list, and the\n\t\tsize and capacity of the list at\n\t\tthe end of the list of floating point values.",
	"I:  Ask for a floating point value, and\n\t\ta position, insert the value at the\n\t\tspecified position. Show the list.",
	"R:  Ask for a position, and remove the\n\t\tvalue at the position, and show the\n\t\tfloating numbers in the list.",
	"H:  Show this menu.",
	"Q:  Exit the program.",
	0
    };

    char choice = ' ';

    Menu m( testMenu );

    DFList * list = NULL;

    while( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch( toupper(choice) ){

	    case 'H' : m.show(); break;
	    case 'Q' : return 0;
	    case 'C' :
		       if( list != NULL) delete list;
		       list = new DFList(5);
		       for( int i = 0; i < 5; i++) list->insert( rand() % 401 / 100.0f, i);
		       break;
	    case 'S' :
		       list->display();
		       cout << endl << "\t\t" << "Size: " << list->size() << endl << "\t\t" << "Capacity: " << list->capacity() << endl << endl;
		      break; 
	    case 'I' :
		      cout << "\t\tEnter a floating point value: ";
		      cin >> value;
		      cout << "\t\tEnter the position: ";
		      cin >> index;
		      list->insert(value, index - 1);
		      list->display();
		      cout << endl << endl;
		      cin.ignore();
		      break;
	    case 'R' :
		      cout << "\t\tEnter the position: ";
		      cin >> index;
		      list->erase(index-1);
		      list->display();
		      cout << endl << endl;
		      cin.ignore();
		      break;
	    default:
		       cout << "\t\tIllegal selection.\n";
	}
    }

    return 0;
}
