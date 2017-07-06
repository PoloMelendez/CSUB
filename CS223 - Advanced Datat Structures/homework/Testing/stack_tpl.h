/* Class template: Stack implemted with linked list
   Major features:
	- Template is used to define stack template.
	- Any number of stacks of different types of entries can be derived
	  from the template.
	- Code are reduced to 1/N if N different stack structures needed.
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

// uncomment the following line if bool is not supported by your compiler.
// enum bool { false, true };

template <class T>
struct StackNode {
	T info;
	StackNode<T> *next;

 	StackNode ( T & t ) { 
		info = t; next = 0;
	}
	~StackNode() { if ( ! next ) delete next; }
};

template <class T>
struct Stack {

	StackNode<T> * top;

    // function members.
	Stack() { top = 0; }
	bool isEmpty() { return top == 0; }
	bool isFull()  { return false; }
	T getTop(  ) { return  top->info; }
	void getTop(T &t ) { t = top->info; }
	int  getElementCount() { return 0; }
	int  getSize() {
		int k = 0;
		StackNode<T> *t = top;
		while ( t ) { k ++; t = t->next; }
		return k;
	}

	void push( T  i ) {
		StackNode<T> * t = new StackNode<T>(i);
		t->next = top; top = t;
	}

	void pop( T & t ) {
		t = top->info;
		StackNode<T> * tmp = top;
		top = top->next;
		tmp->next = 0;
		delete tmp;
	}

	void dump() {
		StackNode<T> * t = top;
		while ( t ) {
			cout << t->info << " ";
			t = t->next;
		}
		cout << endl;
	}

	void clear() { delete top; top = 0; }
} ;

typedef Stack<int> IntStack ;
typedef Stack<char> CharStack ;
typedef Stack<float> FloatStack ;
typedef StackNode<int> IntStackNode;
