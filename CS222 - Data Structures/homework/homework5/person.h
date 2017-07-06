#ifndef PERSON_H
#define PERSON_H

#include<iostream>

using namespace std;

const int STRLEN = 20;

class Person {
    private:
	char name[STRLEN+1];
	int id;
	char email[STRLEN+1];
    
    public:
	Person();
	Person( const Person & );

	Person& operator=( const Person & );

	bool setName( char * );
	bool setID( int );
	bool setEmail( char * );

	const char* getName() const { return name; }
	const int getID() const { return id; }
	const char* getEmail() const { return email; }

	void inputPerson();
	void printPerson()const;
};
#endif
