#ifndef LQUEUE_H
#define LQUEUE_H

#include<iostream>
#include "LList.h"

using namespace std;

template< class T >
class LQueue : LList<T> {

    public:
	LQueue<T>& enque( const T& v ) { this->insert( v, this->count ); return *this; }
	void deque() { this->remove( 0 ); }
	T frnt() { return this->front->data; }
	void show() { LList<T>::show(); }
};
#endif
