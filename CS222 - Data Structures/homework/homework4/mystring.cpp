#include<iostream>
#include<cstdlib>
#include<cstring>
#include "mystring.h"

using namespace std;

myString::myString() {
    str = NULL;
    size = 0;
    count = 0;
    allocateArray(21);
}

myString::myString(const myString& a) {
    str = NULL;
    size = 0;
    count = 0;
    copyObject(a);
}

myString::~myString() {
    if( str != NULL ){ delete[] str; str = NULL; }
}

myString::myString(const char * input ) {
    str = NULL;
    size = 0;
    allocateArray(strlen(input) + 1);
    strcpy(str, input);
    count = strlen(input);
}

myString::myString(const char input ) {
    str = NULL;
    size = 0;
    count = 1;
    allocateArray(21);
    str[0] = input;
}

void myString::resize(int num){
    if(num < 0 || num <= size) return;
    if(str == NULL) allocateArray(num);
    else {
	myString temp = *this;
	allocateArray(num);
	*this = temp;
    }
}

void myString::allocateArray(int num) {
    if(num < 0 || num <= size) return;

    if(str != NULL) {
	delete [] str;
	str = NULL;
	size = 0;
    }

    try {
	str = new char[num];
    } catch(bad_alloc) {
	str = NULL;
    }
    if(str == NULL) {
	cout << "Allocation failed!\n";
	exit(1);
    }

    size = num;
    str[0] = '\0';
}

void myString::copyObject(const myString &source) {
    allocateArray(source.size);
    strcpy(str, source.str);
}

void myString::erase() { 
    if(str != NULL){ 
	delete str; str = NULL;
       count = 0;	
    }
}

void myString::getline(istream& i, int count) {
    allocateArray(count);
    i.getline(str, count);
}

char & myString::operator[](int index) {
    if(index < 0 || index >= size) {
	cout << "Invalid index given.\n";
	exit(1);
    }
    return str[index];
}

myString& myString::operator=(const myString & source){
    if(count != 0) delete[] str;
    copyObject(source);
    count = source.length();
    return *this;
}

istream& operator>>(istream & in, myString & right) {
    int cnt = 0;
    char c;
    in.get(c);
    while(!isspace(c)) {
	    if(cnt >= right.size - 1) {
	    right.resize(right.size + 20);
	    }
	    right.str[cnt++] = c;
	    in.get(c);
    }
    right.str[cnt] = '\0';
    right.count = strlen(right.str);
    return in;
}

ostream& operator<<(ostream & o,const myString & right) {
    if(right.size > 0) o << right.str;
    return o;
}

void myString::operator+=(const myString & right) {
    if(count + right.count <= size)
	allocateArray(count + right.count + 1);
    strcat(str, right.str);
    count = count + right.count;
}

myString operator+(const myString & left, const myString & right) {
    myString temp;
    temp.allocateArray(left.count + right.count + 1);
    strcat(temp.str, left.str);
    strcat(temp.str, right.str);
    return temp;
}
bool operator<(const myString & left, const myString & right) {
    if(left.str == NULL || right.str == NULL) return false;
    int i = strcmp(left.str, right.str);
    return( i < 0 );
}

bool operator>(const myString & left, const myString & right) {
    if(left.str == NULL || right.str == NULL) return false;
    int i = strcmp(left.str, right.str);
    return( i > 0 );
}
bool operator==(const myString & left, const myString & right) {
    if(left.str == NULL || right.str == NULL) return false;
    int i = strcmp(left.str, right.str);
    return( i == 0 );
}
bool operator!=(const myString & left, const myString & right) {
    if(left.str == NULL || right.str == NULL) return false;
    int i = strcmp(left.str, right.str);
    return( i != 0 );
}

