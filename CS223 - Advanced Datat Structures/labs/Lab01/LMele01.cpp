//Leopoldo Melendez

#ifndef DFLIST_H
#define DFLIST_H
#include<iostream>
#include<iomanip>
#include<cassert>
#include<cstring>
using namespace std;

class DFLIST_H
{
    protected:
	float *element;
	int count, cap;

    public:
	DFList(int cap = 10);
	~DFList();
	DFList(const DFList&);

	bool empty() const;

	DFList& insert(float, int pos);

	void display() const;

	int size();
	int capacity();
};
#endif
