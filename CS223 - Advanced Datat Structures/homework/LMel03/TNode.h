#ifndef TNODE_H
#define TNODE_H

#include<iostream>
#include<cassert>
#include<cstring>
#include<iomanip>
#include "LList.h"

using namespace std;

template <class T>
class TNode
{
    public:
	TNode<T> *previous, *next;
	T data;
	TNode(const T& itm){
	    data = itm;
	    previous = next = NULL;}
	~TNode(){
	    if(next != NULL) delete next;}
};
#endif
