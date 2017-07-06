#ifndef LSTACK_H
#define LSTACK_H

#include<iostream>
#include "LList.h"

using namespace std;

template< class T >
class LStack : protected LList<T> {
    
    public:
	LStack<T> & push( const T & v ) { this->insert( v, 0 ); return *this; }
	T pop() { return this->remove( 0 ); }
	T top() { return this->front->data; }
	bool empty() { if(this->count > 0 ) cout << this->count; return false; }
	void show() { LList<T>::show(); }
};
#endif
