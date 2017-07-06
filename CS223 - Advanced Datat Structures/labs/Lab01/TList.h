#ifndef TLIST_H
#define TLIST_H
#include<cctype>
#include<iostream>
#include<assert.h>
using namespace std;

template <class T>
class TList{

    private:
	T *element;
	int capacity, count;

	void expand(){
	}

	void put(T value, int position){
	    if(count == capacity)
		expand;
	    if(position < 0)
		position = 0;
	    if(position > count)
		position = count;
	    shiftRight(position);
	    element[pos] = value;
	    count++;
	}

	void shiftRight(int end){
	    for(int i = count - 1; i >= end; i--)
	    {
		element[i + 1] = element[i];
	    }
	}

	void shiftLeft(int end){
	}

	T remove(int pos){
	    assert(count > 0 && pos >= 0 && pos < count);
	    T tmp = element[pos];
	    shiftLeft(pos);
	    count--;
	    return tmp;
	}

    public:
	TList( int cap = 10 ){
	    element = new T[cap];
	    capacity = cap;
	    count = 0;
	}
	TList( const TList<T> & lst ){
	   capacity =  lst.capacity;
	   count = lst.count;
	   element = new T[capacity];
	   for( int i = 0; i < count; i++)
	       element[i] = lst.element[i];
	       //memcpy( element, lst.element, sizeof(t) * count);
	}

	//void put( T value ){
	   // element[count];
	  //  if(capacity == count)
	//	expand();

	   void show(){
	       cout << "(" << capacity << "," << count << "," << ")";
	       for( int i = 0; i < count; i++)
	       {
		   cout << element[i] << " ";
	       }
	   }
};

#endif
