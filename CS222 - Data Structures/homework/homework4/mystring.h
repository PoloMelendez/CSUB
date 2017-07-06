#ifndef MYSTRING_H
#define MYSTRING_H
#include<iostream>

using namespace std;

class myString{
    private:
	int count, size;
	char * str;
	void resize(int);
	void allocateArray(int);
	void copyObject(const myString &);

    public:
	myString();
	myString(const myString&);
	~myString();
	myString(const char *);
	myString(const char);

	void erase();
	void getline(istream&, int);

	int length()const { return count; }
	int capacity()const { return size; }

	
	void operator+=(const myString &);
	char& operator[]( int );
	myString &operator=(const myString &);

	friend istream& operator>>(istream &, myString &);
	friend ostream& operator<<(ostream &, const myString &);
	friend myString operator+(const myString &, const myString &);
	friend bool operator<(const myString &, const myString &);
	friend bool operator>(const myString&, const myString &);
	friend bool operator==(const myString&, const myString&);
	friend bool operator!=(const myString&, const myString&);
};
#endif
