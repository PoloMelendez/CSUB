#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cstring>

using namespace std;

class OutOfMemory{};
class InvalidIndex{};

template< class T >
class GenericList {
    private:
	T * array;
	int count, cap;

	void allocateArray( int num ){
	    if( num < 0 || num <= count ) return;
	    if( array != NULL ){
		delete[] array;
		array = NULL;
		count = cap = 0;
	    }
	    try{
		array = new T[num]();
	    } catch(bad_alloc){
		array = NULL;
	    }
	    if(array == NULL){
		throw(OutOfMemory());
	    }
	    count = num;
	}

    public:
	GenericList(){
	    count = cap = 0;
	    array = NULL;
	}
	GenericList( int num ){
	    count = cap = num;
	    allocateArray( num );
	}
	GenericList( T * source, int num ){
	    count = cap = num;
	    allocateArray( num );
	    memcpy(array, source, count * sizeof(T));
	}
	GenericList( const GenericList<T>& source ){
	    cap = source.cap;
	    count = source.count;
	    allocateArray( cap );
	    memcpy(array, source.array, count * sizeof(T));
	}
	~GenericList(){
	    if(array != NULL)
		delete[] array;
	}

	int capacity(){ return cap; }
	int size(){ return count; }
	bool empty(){ return count <= 0; }
	n 
	void clear(){
	    if( array != NULL ){
		for( int i = 0; i < cap; i++ )
		    array[i] = 0;
	    }
	}

	void reserve( int newCap ){
	    if( newCap > cap ){
		T * temp = new T[newCap];
		memcpy(temp, array, count *sizeof(T));
		swap(array, temp);
		delete[] temp;
		cap = newCap;
	    }
	}

	void resize( int newCount ){
	    if( newCount == count) return;
	    if( newCount < count ){
		for( int i= newCount; i < count; i++)
		    array[i] = 0;
	    }
	    else if( newCount > count && newCount < cap ){
		for( int i = count-1; i < newCount; i++)
		    array[i] = 0;
	    }
	    else
		reserve( newCount );
	    count = newCount;
	}

	void push_back( const T& v ){
	    if( count == cap ){
		cap = cap + 10;
		T * tmp = new T[cap];
		memcpy(tmp, array, count*sizeof(T));
		swap( tmp, array);
		delete[] tmp;
	    }
	    array[count] = v;
	    count++;
	}

	T pop_back(){
	    if(empty()){
		cout<< "Stack is empty, cannot pop!\n";
		return -1;
	    }
	    T data = array[count-1];
	    array[count-1] = 0;
	    count--;
	    return data;
	}

	int find( const T& v ){
	    for(int i = 0; i < count; i++){
		if(array[i] == v)
		    return i;
	    }
	    return -1;
	}

	GenericList& operator=( const GenericList& source ){
	    if(this == &source)
		return *this;
	    allocateArray( source.cap );
	    memcpy(array, source.array, count*sizeof(T));
	    cap = source.cap;
	    count = source.count;
	}

	friend ostream& operator<<( ostream& out,const GenericList<T>& lst ){
	    out << "[ " << lst.count << ", " << lst.cap << " ]";
	    for(int i=0; i< lst.count; i++){
		out << lst.array[i] << " ";
	    }
	    return out;
	}
	T& operator[]( int index ){
	    if(index < 0 || index > count-1)
		throw(InvalidIndex());
	    return array[index];
	}
};

template <class T>
void subMenu(GenericList<T> &list)
{
    T elem;
    int sz, sel;
    bool badIndex = true;

    do
    {
	cout << endl;
	cout << "            GenericList Sub-Menu \n";
	cout << "========================================================================\n";
	cout << "1.  Print the capacity and size of the list\n";
	cout << "2.  Clear all the elements on the list\n";
	cout << "3.  Add an element to the end of the list\n";
	cout << "4.  Remove the last element from the list and print its value \n";
	cout << "5.  Use the index operator to set and print the value of an element\n";
	cout << "6.  Use the reserve function to change the number of elements allocated \n";
	cout << "7.  Use the resize function to change the number of elements in use\n";
	cout << "8.  Search for a given element in the list\n";
	cout << "9.  Print the current contents of the list\n\n";
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
		if(list.empty()){
		    cout << "List is empty: Operation Failed!\n";
		    break;
		}
		cout << "\nCapacity :" << list.capacity() << endl;
		cout << "\nSize :" << list.size() << endl;
		break;
	    case 2:
		if(list.empty()){
		    cout << "List is empty: Operation Failed!\n";
		    break;
		}
		list.clear();
		break;
	    case 3:
		cout << "Enter a value: ";
		try{
		    cin >> elem;
		    list.push_back( elem );
		} catch(OutOfMemory){
		    cout << "Out of Memory!" << endl;
		    sel = 0;
		}
		break;
	    case 4:
		if(list.empty()){
		    cout << "List is empty: Operation Failed!\n";
		    break;
		}
		cout << "Popped element: " << list.pop_back();
		break;
	    case 5:
		do{
		cout << "Enter the index you wish to access: ";
		cin >> sz;
		try{
		    cout << "list[" << sz << "] = " << list[sz] << endl;
		    cout << "Enter the new element: ";
		    cin >> list[sz];
		    cout << "list[" << sz << "] = " << list[sz] << endl;
		    badIndex = true;
		} catch(InvalidIndex){
		    cout << "Bad index!" << endl;
		    badIndex = false;
		}
		}while(!badIndex);
		cout << endl;
		break;
	    case 6:
		cout << "Enter the new Cap: " << endl;
		cin >> sz;
		try{
		list.reserve( sz );
		cout << "Cap after reserve: " << list.capacity() << endl;
		} catch(OutOfMemory){
		    cout << "Out of Memory!" << endl;
		    sel = 0;
		}
		break;
	    case 7:
		cout << "Enter the new Count: ";
		cin >> sz;
		try{
		    list.resize( sz );
		} catch(OutOfMemory){
		    cout <<"Out of Memory!" << endl;
		    sel = 0;
		}
		break;
	    case 8:
		if(list.empty()){
		    cout << "List is empty: Operation Failed!\n";
		    break;
		}
		cout << "Enter the element to search for: ";
		cin >> elem;
		sz = list.find( elem );
		if( sz == -1 )
		    cout << "The element was not found!\n";
		else
		    cout << "The element was found at index " << sz << endl;
		break;
	    case 9:
		if(list.empty()){
		    cout << "List is empty: Operation Failed!\n";
		    break;
		}
		cout << list << endl;
		break;
	    case 0:
		break;
	}

    } while(sel != 0);
}

int main()
{
    int sel;
    GenericList<int> iList;
    GenericList<double> dList;
    GenericList<char> cList;

    do
    {
	cout << endl;
	cout << "    Welcome to the CS222 Homework 6 Menu\n";
	cout << "==============================================\n";
	cout << "1.  Test the GenericList class for integers\n";
	cout << "2.  Test the GenericList class for doubles\n";
	cout << "3.  Test the GenericList class for characters\n\n";
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

	    case 1:  // Invoke the subMenu with an integer
		subMenu(iList);
		break;

	    case 2:  // Invoke the subMenu with a double
		subMenu(dList);
		break;

	    case 3:  // Invoke the subMenu with a character
		subMenu(cList);
		break;

	    default:
		cout << "That is an invalid selection!\n";
	}
    } while(sel != 0);

    return 0;
}
