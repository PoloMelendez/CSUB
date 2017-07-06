#ifndef LLIST_H
#define LLIST_H

#include<iostream>
#include<cassert>
#include<cstring>
#include<iomanip>
#include "TNode.h"

using namespace std;

template <class T>
class LList
{
    protected:
	TNode<T> *front, *rear;
	int count;
    
    public:
	LList();//LList lst; to call this constructor
	LList(const LList&);//LList lst2(lst); to call cpy constructor
	~LList();//use to return used memory

	int size()const{
	    return count;
	}
	
	bool empty()const{
	    if(count > 0) return false;
	    else return true;
	}

	//can make it void, return the new count, or
	//return the object that called it for cascading calls.
	LList& insert(const T&, int);
	
	T remove( int );

	void showForwards()const;
	void showBackwards()const;
	
};
#endif
