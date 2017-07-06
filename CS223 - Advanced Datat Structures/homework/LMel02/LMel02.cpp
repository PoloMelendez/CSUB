//Leopoldo Melendez
//Cannot compile for some reason due to the .h and the .cpp files.
//Looked up how to resolve this issue, but only answers I could find
//were to put everything in the .h file.
#include<iostream>
#include<iomanip>
#include<cctype>
#include<cstdlib>
#include<ctime>
#include "Menu.h"
#include "DTList.h"
#include "DTList.cpp"
#include<time.h>

using namespace std;

template<class C>
ostream& operator << (ostream & out, const DTList<C>& lst){
    out << "[ " << lst.count << ", " << lst.cap << " ] ";
    for(int i = 0;i < lst.count; i++)
    {
	out << lst.element[i] << " ";
    }
    return out;
}
int main() {

    int intValue, intIndex, charIndex;
    char charValue;

    const char * testMenu[] = {
	"F: Create one integer and one character\n\t\tList, and insert 10 random values in\n\t\teach list. The value ranges of lists\n\t\t0 ... 99 and 'A' .. 'Z'.",
	"S:  Show two lists by using operator<<.",
	"A:  Ask for an int and a char value,\n\t\tinsert the values at random positions\n\t\tof the lists.",
	"T:  Test copy constructor, and list\n\t\tmember access operator[].",
	"D:  Generate a random position, and\n\t\tremove values at the positions of the\n\t\tint and char lists.",
	"H:  Show this menu.",
	"Q:  Exit the program.",
	0
    };

    char choice = ' ';
    srand(time(NULL));

    Menu m( testMenu );

    DTList<int> * list1 = NULL;
    DTList<char> * list2 = NULL;
    DTList<int> * list3 = NULL;

    while( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch( toupper(choice) ){

	    case 'H' : m.show(); break;
	    case 'Q' : return 0;
	    case 'F' :
		       if( list1 != NULL || list2 != NULL)
		       {
			   delete list1;
			   delete list2;
		       }
		       list1 = new DTList<int>(10);
		       list2 = new DTList<char>(10);
		       for( int i = 0; i < 10; i++)
		       {
			   list1->insert(rand()%100 + 1, i);
			   list2->insert((char)(rand()%26) + 'A', i);
		       }
		       cout << endl;
		       break;
	    case 'S' :
		       cout << "\t\t" << *list2 << endl;
		       cout << "\t\t" << *list1 << endl;
		       cout << endl;
		       break;
	    case 'A' :
		       cout << "\t\tEnter an int value: ";
		       cin >> intValue;
		       cout << "\t\tEnter a char value: ";
		       cin >> charValue;
		       cin.ignore();
		       intIndex = rand()%list1->size();
		       charIndex = rand()%list2->size();
		       list1->insert(intValue, intIndex);
		       list2->insert(charValue, charIndex);
		       break;
	    case 'T':
		       if( list3 != NULL) delete list3;
		       list3 = new DTList<int>(*list1);
		       cout << "\t\tList 1 = " << *list1 << endl 
			  << "\t\tList 3 = " << *list3 << endl << endl;
		       break;
	    case 'D' :
		      intIndex = rand()%list1->size();
		      charIndex = rand()%list2->size();
		      list1->erase(intIndex);
		      list2->erase(charIndex);
		      break;
	    default:
		       cout << "\t\tIllegal selection.\n";
	}
    }

    return 0;
}
