//Leopoldo Melendez
#include<iostream>
#include<iomanip>
#include<cctype>
#include<cstdlib>
#include<ctime>
#include "Menu.h"
#include "LList.h"
#include "LList.cpp"
#include<time.h>

using namespace std;

void createLists(LList<double>*, LList<int>*);
void printLists(LList<double>*, LList<int>*);
void randomInsertion(LList<double>*, LList<int>*);
void randomRemoval(LList<double>*, LList<int>*);

int main() {

    const char * testMenu[] = {
	"F/f: Create one double and one integer\n\t\tlist, and insert 10 random values in\n\t\teach list. The value ranges of lists\n\t\tare 0.00 .. 4.00 and 10 .. 99.",
	"S/s:  Show two lists in both directions",
	"A/a:  Ask for a double and an integer,\n\t\tinsert the values at random positions\n\t\tof the lists.",
	"D/d:  Generate a random position, and\n\t\tremove values at the positions of the\n\t\ttwo lists. Show values removed",
	"H/h/?:  Show this menu.",
	"Q/q:  Exit the program.",
	0
    };

    char choice = ' ';
    srand(time(NULL));

    Menu m( testMenu );

    LList<double> * list1 = NULL;
    LList<int> * list2 = NULL;

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
		       list1 = new LList<double>();
		       list2 = new LList<int>();
		       createLists(list1, list2);
		       break;
	    case 'S' :
		       printLists(list1, list2);
		       break;
	    case 'A' :
		       randomInsertion(list1, list2);
		       break;
	    case 'D' :
		       randomRemoval(list1, list2);
		       break;
	    default:
		       cout << "\t\tIllegal selection.\n";
	}
    }

    return 0;
}

void createLists(LList<double>* lst, LList<int>* lst2){
    for(int i = 0;i < 10;i++){
	lst->insert((rand() % 400)/100.0, i);
	lst2->insert(rand() % 90 + 10, i);
    }	
}

void printLists(LList<double>*lst1, LList<int>*lst2){
    if(lst1 != NULL && lst2 != NULL){
	cout << fixed << setprecision(2);
	cout << "[ List 1, Forwards  ] ";
	lst1->showForwards();
	cout << endl;
	cout << "[ List 1, Backwards ] ";
	lst1->showBackwards();
	cout << endl;
	cout << "[ List 2, Forwards  ] ";
	lst2->showForwards();
	cout << endl;
	cout << "[ List 2, Backwards ] ";
	lst2->showBackwards();
	cout << endl;
	cout << endl;
    }
    else
	cout << "\t\tNo lists created! Use option F/f first" << endl << endl;
}

void randomInsertion(LList<double>* lst1,LList<int>* lst2){
    double temp1;
    int temp2;
    cout << "\t\tEnter a double: ";
    cin >> temp1;
    cout << "\t\tEnter an int: ";
    cin >> temp2;

    if(lst1 != NULL && lst2 != NULL){
	lst1->insert(temp1, rand() % lst1->size());
	lst2->insert(temp2, rand() % lst2->size());
	cout << endl;
    }

    else{
	lst1->insert(temp1, 0);
	lst2->insert(temp2, 0);
    }
    cin.ignore();
}

void randomRemoval(LList<double>* lst1, LList<int>* lst2){
    if(lst1 != NULL && lst2 != NULL){
	int random1, random2;
	random1 = rand() % lst1->size();
	random2 = rand() % lst2->size();
	cout << "\t\t" << lst1->remove(random1) << " Removed from list1." << endl;
	cout << "\t\t" << lst2->remove(random2) << " Removed from list2." << endl;
	cout << endl;
    }
    else
	cout << "\t\tNo lists created! Use option F/f first" << endl << endl;
}
