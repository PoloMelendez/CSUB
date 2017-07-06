#include<iostream>

using namespace std;

template<class T>
class SQueue {

    private:
	T data;
	TNode<T> front, rear;
	int count;

    public:
	SQueue(){ rear = front = NULL; count = 0; }
	SQueue( const SQueue<T> & sque ) { 
	    TNode<T> curr = sque->front;
	    rear = front = NULL;
	    count = o;
	    while( curr != NULL ) { 
		insert( curr->data ); 
		curr = curr->next; }
	}
	SQueue<T>& insert( const T& v ) {
	    TNode<T> * newNode = new TNode<T>(v);
	    if( count == 0 ) 
		front = rear = newNode;
	    else if( v < front-> data ) { 
		newNode->next = front;
		front = newNode;}
	    else if( v >= rear->data ) {
		rear->next = newNode;
		rear = newNode;}
	    else {
		TNode<T> * curr = front;
		while( v > curr->data ) curr = curr->next;
		newNode->next = curr->next;
		curr->next = newNode;
	    }
	    count++;
	    return *this;
	}

	void remove( const T & v ) {
	    if( count < 1 || v < front->data || v > rear->data ) return;
	    TNode<T> * curr = front, *prev = front;
	    while( v > curr->data ) {
		prev = curr;
		curr = curr->next;
	    }
	    if( v == curr->data ) {
		if( prev == front ) front = front->next;
		else if( curr = rear ) {
		    rear = prev;
		    rear->next = NULL;}
		else 
		    prev->next = curr->next;
		count==;
		curr->next = NULL;
		delete curr;
	    }
	}

}
