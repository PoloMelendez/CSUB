
using namespace std;

#include "DTList.h"

template<class T>
DTList<T>::DTList( int cap )
{
    this->cap = cap;
    this-> count = 0;
    this-> element = new T[cap];
}

template<class T>
DTList<T>::DTList( const DTList<T> & lst)
{
    this->cap = lst.cap;
    this->count = lst.count;
    this->element = new T[cap];
    memcpy(this->element, lst.element, count * sizeof(T));
}

template<class T>
DTList<T>::~DTList(){
    delete[] element;
}

template<class T>
DTList<T> & DTList<T>::insert( const T & itm, int pos ){
    if(pos < 0) pos = 0;
    if(pos > count) pos = count;
    if(count == cap) expand();
    shiftRight(pos);
    element[pos] = itm;
    count++;
    return *this;
}

template<class T>
T DTList<T>::erase( int pos ){
    assert(pos >= 0 && pos < count);
    T v = element[pos];
    for(int i = pos; i < count; i++)
	element[i] = element[i + 1];
    count--;
    return v;
}

template<class T>
void DTList<T>::expand(){
    cap *= 2;
    T *pf = new T[cap];
    memcpy(pf, element, count *sizeof(T));
    swap(element, pf);
    delete[] pf;
}

template<class T>
void DTList<T>::shiftRight(int end){
    for(int i=count - 1;i >= end;i--)
	element[i + 1] = element[i];
}

template<class T>
int DTList<T>::size()const{
    return count;
}

template<class T>
int DTList<T>::capacity()const{
    return cap;
}

template<class T>
void DTList<T>::display() const{
    cout << "\t\t";
    for(int i = 0;i < count; i++)
	cout << element[i] << "  ";
}

template<class T>
T & DTList<T>::operator [] ( int k ){
    assert(k >= 0 && k < count);
    return element[k];
}

template<class T>
T DTList<T>::rand(T lb, T ub){
    return(T)(rand()%(ub - lb + 1) + lb);
}


