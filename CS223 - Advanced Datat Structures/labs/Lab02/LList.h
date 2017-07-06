//Leopoldo Melendez
#ifndef LLIST_H
#define LLIST_H

#include<iostream>
#include<cassert>
#include<cstring>
#include<iomanip>

using namespace std;

class LList
{
    private:
	Node * front, * rear;
	int count;
	
    public:
	LList()
	{
	    front = NULL;
	    rear = NULL;
	    count = 0;
	}
	~LList();

	void insert( int v, int pos)
	{
	    Node* newNode = new Node(v);
	    Node* temp = front;
	    
	    if(first == NULL)
	    {
		newNode->next = NULL;
		front = newNode;
		rear = newNode;
	    }
	    else if(pos > 0 && pos < count)
	    {
		for(int i = 0;i < pos;i++)
		{
		    temp = temp->next;
		}
		newNode->next = temp->next;
		temp->next = newNode;
	    }
	    else if(pos == 0)
	    {
		newNode->next = front;
		front = newNode;
	    }
	}

	int remove( int pos );

	void show();

	int size();
}
#endif
