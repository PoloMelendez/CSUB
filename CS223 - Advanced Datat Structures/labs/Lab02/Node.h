//Leopoldo Melendez
#ifndef NODE_H
#define NODE_H

#include<iostream>
#include<cassert>
#include<cstring>
#include<iomanip>

using namespace std;

class Node
{
    public:
	int data;
	Node * next;
	
	Node( int& data );
	~Node();
}
#endif
