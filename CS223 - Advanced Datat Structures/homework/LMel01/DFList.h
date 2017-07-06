//Leopoldo Melendez
//
#ifndef DFLIST_H
#define DFLIST_H

#include<iostream>
#include<iomanip>
#include<cassert>
#include<cstring>

using namespace std;

class DFList
{
    protected:
	float * element;
	int count, cap;

    private:
	void expand();
	void shiftRight(int end);

    public:
	DFList(int cap = 10);
	~DFList();
	DFList(const DFList& lst);
	
	bool empty() const;

	DFList& insert(float item, int pos);

	float erase(int pos);

	void display() const;

	int size();
	int capacity();
};
#endif
