#ifndef DTLIST_H
#define DTLIST_H

#include<iostream>
#include<iomanip>
#include<cassert>
#include<cstring>

using namespace std;

template <class T>
class DTList
{
    protected:
	T * element;
	int count, cap;

    private:
	void shiftRight( int end );
	void expand();
	T rand(T lb, T ub);

    public:
	DTList( int cap = 10 );
	~DTList();
	DTList( const DTList<T> & lst);

	bool empty() const;

	DTList<T> & insert( const T & itm, int pos);

	T erase( int pos );

	void display() const;

	T & operator [] ( int k );

	template<class C> 
	friend ostream& operator << (ostream& out, const DTList<C>& lst );

	int size()const;
	int capacity()const;
};
#endif
