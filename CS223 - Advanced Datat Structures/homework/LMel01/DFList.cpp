//Leopoldo Melendez
#include<iostream>
#include<iomanip>
#include<cassert>
#include<cstring>

using namespace std;

#include "DFList.h"

DFList::DFList(int capacity){
    cap = capacity;
    count = 0;
    element = new float[cap];
}

DFList::DFList(const DFList & lst){
    cap = lst.cap;
    count = lst.count;
    element = new float[cap];
    memcpy(element, lst.element, count * sizeof(float));
}

DFList::~DFList(){}

DFList & DFList::insert(float itm, int pos){
    if(pos < 0) pos = 0;
    if(pos > count) pos = count;
    if(count == cap) expand();
    shiftRight(pos);
    element[pos] = itm;
    count++;
    return *this;
}

float DFList::erase(int pos){
    assert(pos > 0 && pos < count);
    float v = element[pos];
    for(int i = pos;i < count; i++)
	element[i] = element[i+1];
    count--;
    return v;
}

void DFList::expand(){
    cap *= 2;
    float *pf = new float[cap];
    memcpy(pf, element, count *sizeof(float));
    swap(element, pf);
    delete[] pf;
}

void DFList::shiftRight(int end){
    for(int i=count;i >= end;i--)
	element[i + 1] = element[i];
}

int DFList::size(){
    return count;
}

int DFList::capacity(){
    return cap;
}

void DFList::display() const{
    cout << "\t\t";
    for(int i = 0;i < count; i++)
	cout  << element[i] << "  ";
} 
