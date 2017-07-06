#ifndef LLIST_H
#define LLIST_H

#include<iostream>
#include<cassert>

using namespace std;

template< class T >
class Node {

    public:
	T data;
	Node<T> *next, *previous;
	Node( const T &v ) { data = v; next = NULL; }
	~Node() { if(next != NULL) delete next; }
};

template< class T >
class LList {
    protected:
	Node<T> *front, *rear;
	int count;
	LList& insert( const T& item, int pos ) {
	    Node<T> *newNode = new Node<T>(item);
	    if(pos < 0) pos = 0;
	    if( count < 1 ) rear = front = newNode;
	    else if( pos < 1 ) {
		newNode->next = front;
		front->previous = newNode;
		front = newNode;
	    }
	    else if( pos > count ) pos = count;
	    else if( pos == count ) {
		rear->next = newNode;
		newNode->previous = rear;
		rear = newNode;
	    }
	    else {
		Node<T> *currentNode = front;
		for( int i = 1;i < pos;i++ )
		    currentNode = currentNode->next;
		newNode->next = currentNode->next;
		newNode->previous = currentNode;
		currentNode->next = newNode;
		newNode->next->previous = newNode;
	    }
	    count++;
	    return *this;
	}

	T remove( int pos ){
	    assert( pos >= 0 && pos < count );
	    T v = front->data;
	    Node<T> * current = front;
	    if(count == 1) front = rear = NULL;
	    else if(pos == 0){
		front = front->next;
		front->previous = NULL;}
	    else if(pos == count - 1){
		current = rear;
		v = current->data;
		rear = rear->previous;
		rear->next = NULL;}
	    else{
		for( int i = 0;i < pos; i++)
		    current = current->next;
		current->previous->next = current->next;
		current->next->previous = current->previous;
		v = current->data;}
	    count--;
	    current->next = current->previous = NULL;
	    delete current;
	    return v;
	}

    public:
	LList(){ count = 0; rear = front = NULL; }
	LList( const LList & lst ) {
	    count = 0; rear = front = NULL;
	    Node<T> * curr = lst.front;
	    while( curr != NULL ) {
		this->insert( curr->data, count );
		curr = curr->next;}
	}
	~LList(){ if( front != NULL ) delete front; }

	T frnt() { return front->data; }

	int size()const { return count; }
	bool empty()const { return count <= 0; }

	void show() { 
	    Node<T> * temp = front;
	    while(temp!= NULL) {
		cout << temp->data << " ";
		temp=temp->next;
	    }
	    cout << endl;
	}
};
#endif
