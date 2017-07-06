#ifndef LSQUEUE_H
#define LSQUEUE_H

#include<iostream>
#include "LList.h"

using namespace std;

template< class T >
class LSQueue : protected LList<T> {

    public:
	LSQueue<T> & enque( const T& v ){
	    Node<T> * nn = new Node<T>( v );
	    if( this->empty() ) {
		//this->front = this->rear = nn;
		this->insert( v, 0 );
	    }
	    else if( v <= this->front->data ) this->insert( v, 0 );
	    else if( v >= this->rear->data ) this->insert(v, this->count );
	    else {
		int pos = 1;
		Node<T> * curr = this->front;
		while( curr->next->data < v ) {
		    pos++;
		    curr = curr->next;
		}
		this->insert( v, pos );
	    }
	    return *this;
	}

	void deque( const T& v ){
	    if( this->count <= 0 || v < this->front->data || v > this->rear->data) return;
	    else if( v == this->front->data ) { this->remove( 0 ); return; }
	    else if( v == this->rear->data ) { this->remove( this->count-1 ); return; }
	    else {
		int pos = 1;
		Node<T> * curr = this->front;
		while( curr->next->data < v ){
		    pos++;
		    curr = curr->next;
		}
		this->remove(  pos );
		/*
		Node<T> * curr = this->front;
		while( curr->next->data < v ) curr = curr->next;
		if( curr->next->data != v ) return;
		Node<T>* temp = curr->next;
		curr->next = curr->next->next;
		temp->next = NULL;
		delete temp;
		this->count--;*/
	    }
	}

	T frnt() { return this->front->data; }

	void show() { LList<T>::show();}
};
#endif
