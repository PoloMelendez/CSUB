#ifndef TQUEUE_H
#define TQUEUE_H
#include<iostream>
#include<cassert>

using namespace std;

template <class T>
class TNode {
    public:
	T data;
	TNode<T> * next;
	TNode( T value ) { data = value; next = NULL; }
	~TNode() { delete next; }
};

template <class T>
class TQueue {
    protected:

	TNode<T> * front;
	int count;
    
    public:

	TQueue() { front = NULL; count = 0; }
	~TQueue() { delete front; }

	TQueue<T> & enque ( const T & value ) {
	    TNode<T> *newNode = new TNode<T>( value );
	    newNode->next = front;
	    front = newNode;
	    count++;
	    return *this;
	}

	void deque() { 
	    assert( count > 0 );
	    TNode<T> * current = front; 
	    front = front->next = 0;
	    delete current;
	    count--;
	}

	bool empty()const { return front==NULL; }
	T frnt()const { return front->data; }
	int size()const { return count; }
};
#endif
