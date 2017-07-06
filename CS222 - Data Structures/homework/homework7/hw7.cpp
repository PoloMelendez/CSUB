#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cstring>
#include<list>
#include<vector>
#include "iterator"
#include<algorithm>

using namespace std;

class OutOfMemory{};
class InvalidIndex{};


template <class T>
void subMenu(list<T> &lst)
{
    T elem;
    int sz, sel;
    bool found = true;
    typename list<T>::iterator it;
    do
    {
	cout << endl;
	cout << "            list Sub-Menu \n";
	cout << "========================================================================\n";
	cout << "1.  Insert a value at the front of the list\n";
	cout << "2.  Insert a value at the back of the list\n";
	cout << "3.  Insert a value at a given position in the list\n";
	cout << "4.  Search the list for a value \n";
	cout << "5.  Delete all instances of a value\n";
	cout << "6.  Print the list contents \n\n";
	cout << "0.  Return to the main menu\n";
	cout << "========================================================================\n";
	cout << "Enter selection: ";
	cin >> sel;
	while(cin.fail())
	{
	    cin.clear();
	    cin.ignore(20, '\n');
	    cout << "The selection must be an integer.\nEnter selection: ";
	    cin >> sel;
	}
	cout << endl;
	switch(sel) {
	    /*------------------------------------------------*/
	    /* Complete the switch statement for the submenu  */
	    /*------------------------------------------------*/
	    case 1:
		cout << "Enter a value to insert at the front: ";
		cin >> elem;
		lst.push_front( elem );
		cout << endl;
		cin.ignore();
		break;
	    case 2:
		cout << "Enter a value to insert at the back: ";
		cin >> elem;
		lst.push_back( elem );
		cin.ignore();
		cout << endl;
		break;
	    case 3:
		it = lst.begin();
		cout << "Enter a value to insert: ";
		cin >> elem;
		cout << "Enter the index to insert into: ";
		cin >> sz;
		for(int c = 0; c < sz; c++)
		    it++;
		lst.insert( it, elem );
		cin.ignore();
		cout << endl;
		break;
	    case 4:
		found = false;
		cout << "Enter the value you wish to search for: ";
		cin >> elem;
		for( it = lst.begin(); it != lst.end(); it++ ){
		    if( *it == elem )
			found = true;
		}
		if( found )
		    cout << elem << " found in the list" << endl;
		else
		    cout << elem << " not found in the list" << endl;
		cin.ignore();
		cout << endl; cin.get();

		break;
	    case 5:
		cout << "Enter the value you wish to remove: ";
		cin >> elem;
		lst.remove( elem );
		cout << endl; cin.get();
		break;
	    case 6:
		for( it = lst.begin(); it != lst.end(); it++ )
		    cout << *it << " ";
		cout << endl;
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 0:
		break;
	}

    } while(sel != 0);
}

template <class T>
void subMenu(vector<T> &vec)
{
    T elem;
    int sz, sel;
    bool badIndex = true;
    typename vector<T>::iterator it;
    do
    {
	cout << endl;
	cout << "            Vector Sub-Menu \n";
	cout << "========================================================================\n";
	cout << "1.  Print the capacity and size of the vector\n";
	cout << "2.  Clear all the elements on the vector\n";
	cout << "3.  Add an element to the end of the vector\n";
	cout << "4.  Remove the last element from the vector and print its value \n";
	cout << "5.  Use the index operator to set and print the value of an element\n";
	cout << "6.  Use the reserve function to change the number of elements allocated \n";
	cout << "7.  Use the resize function to change the number of elements in use\n";
	cout << "8.  Search for a given value in the vector using STL find()\n";
	cout << "9.  Print the current contents of the vector using the [] operator\n\n";
	cout << "0.  Return to the main menu\n";
	cout << "========================================================================\n";
	cout << "Enter selection: ";
	cin >> sel;
	while(cin.fail())
	{
	    cin.clear();
	    cin.ignore(20, '\n');
	    cout << "The selection must be an integer.\nEnter selection: ";
	    cin >> sel;
	}
	cout << endl;
	switch(sel) {
	    /*------------------------------------------------*/
	    /* Complete the switch statement for the submenu  */
	    /*------------------------------------------------*/
	    case 1:
		cout << "Capacity: "<< vec.capacity() << endl;
		cout << "Size: "<< vec.size() << endl;
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 2:
		vec.clear();
		break;
	    case 3:
		cout << "Enter the element to insert at the end: ";
		cin >> elem;
		vec.push_back( elem );
		cin.ignore();
		cout << endl;
		break;
	    case 4:
		cout << "The element being removed is: " << vec.back() << endl;
		vec.pop_back();
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 5:
		cout << "Enter the index to insert into: ";
		cin >> sz;
		cout << "The current element at index " << sz 
		    << " is " << vec[sz] << endl;
		cout << "Enter the new element: ";
		cin >> elem;
		vec[sz] = elem;
		cout << "The element at index " << sz << " is now "
		    << vec[sz] << endl;
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 6:
		cout << "Enter the desired capacity: ";
		cin >> sz;
		vec.reserve( sz );
		cout << endl;
		cin.ignore();
		break;
	    case 7:
		cout << "Enter the number of elements to keep in use: ";
		cin >> sz;
		vec.resize( sz );
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 8:
		cout << "Enter the element to search for: ";
		cin >> elem;
		it = find(vec.begin(), vec.end(), elem);
		if(it == vec.end())
		    cout << elem << " was not found" << endl;
		else
		    cout << elem << " is in the vector" << endl;
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 9:
		for( int i = 0; i < vec.size(); i++ )
		    cout << vec[i] << " ";
		cout << endl;
		cin.ignore();
		cout << endl; cin.get();
		break;
	    case 0:
		break;
	}

    } while(sel != 0);
}

int main()
{
    int sel;
    list<int> intList;
    list<double> doubleList;
    vector<int> intVector;
    vector<double> doubleVector;

    do
    {
	cout << endl;
	cout << "    Welcome to the CS222 Homework 6 Menu\n";
	cout << "==============================================\n";
	cout << "1.  Test the vector STL with integers\n";
	cout << "2.  Test the vector STL with doubles\n";
	cout << "3.  Test the list STL with integers\n";
	cout << "4.  Test the list STL with doubles\n";
	cout << "0.  Exit the program\n";
	cout << "==============================================\n";
	cout << "Enter selection: ";
	cin >> sel;
	while(cin.fail())
	{
	    cin.clear();
	    cin.ignore(20, '\n');
	    cout << "The selection must be an integer.\nEnter selection: ";
	    cin >> sel;
	}

	switch(sel)
	{
	    case 0:
		cout << "Good bye!\n";
		break;

	    case 1:
		subMenu(intVector);
		break;

	    case 2:
		subMenu(doubleVector);
		break;

	    case 3:
		subMenu(intList);
		break;
	    case 4:
		subMenu(doubleList);
		break;

	    default:
		cout << "That is an invalid selection!\n";
	}
    } while(sel != 0);

    return 0;
}
