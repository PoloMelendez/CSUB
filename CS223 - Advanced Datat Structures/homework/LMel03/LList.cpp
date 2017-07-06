
using namespace std;

#include "LList.h"

//default constructor
template<class T>
LList<T>::LList()
{
    count = 0;
    rear = front = NULL;
}

//copy constructor
template<class T>
LList<T>::LList(const LList<T> & lst)
{
    count = 0;
    rear = front = NULL;
    TNode<T> *currentNode = lst.front;
    while(currentNode != NULL)
    {
	this->insert( currentNode->data, count );
	currentNode = currentNode->next;
    }
}

//destructor
template<class T>
LList<T>::~LList()
{
    if( front != NULL ) delete front;
}

//insert
template<class T>
LList<T>& LList<T>::insert(const T& item, int pos){
    TNode<T> *newNode = new TNode<T>(item);
    if( count < 1 ) rear = front = newNode; //first item
    else if( pos < 1 ) //insert at front
    {
	newNode->next = front;
	front->previous = newNode;
	front = newNode;
    }
    else if( pos > count ) pos = count;//keeps pos <= count
    else if( pos == count ) //insert at the rear
    {
	rear->next = newNode;
	newNode->previous = rear;
	rear = newNode;
    }
    else //insert a new node between two existing nodes
    {
	TNode<T> *currentNode = front;
	for( int i = 1;i < pos;i++ )
	    currentNode = currentNode->next;
	newNode->next = currentNode->next;
	newNode->previous = currentNode;
	currentNode->next = newNode;
	newNode->next->previous = newNode;
    }
    count++;    
}

//remove
template<class T>
T LList<T>::remove( int pos ){
    assert( pos >= 0 && pos < count );
    T v = front->data;
    TNode<T> * current = front;
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

//show
template<class T>
void LList<T>::showForwards()const{
    TNode<T>* tmp = front;
    while(tmp != NULL){
	cout << tmp->data << " ";
	tmp = tmp->next;
    }
}

template<class T>
void LList<T>::showBackwards()const{
    TNode<T>* tmp = rear;
    while(tmp != NULL){
	cout << tmp->data << " ";
	tmp = tmp->previous;
    }
}
