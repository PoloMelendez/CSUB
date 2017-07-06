#ifndef TSTACK_H
#define TSTACK_H
#include<iostream>

using namespace std;

template <class T>
class TNode {

    public:

	static int cnt;
	T data;
	TNode<T> * next;
	int myCnt;

	TNode( T value ) { 
	    data = value; 
	    next = NULL; 
	    myCnt = cnt++; 
	}
	~TNode() { 
	    if( next != NULL ) delete next;
	}
};

template <class T>
int TNode<T>::cnt = 1;

template <class T>
class TStack {

    protected:

	TNode<T> * front;
	int count;

    public:

	TStack() { 
	    front = NULL; count = 0; 
	}
	~TStack() { 
	    if( front!= NULL) delete front; 
	}

	void push( T v ) {
	    TNode<T> * ptr = new TNode<T> ( v );
	    ptr->next = front;
	    front = ptr;
	    count++;
	}

	T top()const{
	    if( !empty() )
		return( front->data );
	    else {
		cout << "Stack is empty \nReturning garbage\n";
		T * temp = new( T );
		T garbage = *temp;
		delete temp;
		return garbage;
	    }
	}

	bool empty()const {
	    return ( front == NULL );
	}

	T pop() {
	    T num;
	    if ( !empty() )
	    {
		num = front->data;
		TNode<T> * ptr = front;
		front = front->next;
		count--;
		ptr->next = NULL;
		delete ptr;
	    }
	    else
		cout << "Stack is empty, can't remove a value\n";
	    return num;
	}

	bool clear() {
	    if ( this->front != NULL ) delete this->front;
	    this->front = NULL;
	}
};
#endif
