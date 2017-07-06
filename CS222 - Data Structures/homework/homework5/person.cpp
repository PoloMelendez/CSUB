#include<iostream>
#include<iomanip>
#include<cstring>
#include "person.h"
using namespace std;

Person::Person() {
    strncpy( name, "", STRLEN+1 );
    strncpy( email, "", STRLEN+1);
    id = 0;
}

Person::Person( const Person &source ) {
    strncpy( name, source.name, STRLEN );
    strncpy( email, source.email, STRLEN );
    id = source.id;
}

Person &Person::operator=( const Person &source ) {
    strncpy( name, source.name, STRLEN );
    strncpy( email, source.email, STRLEN );
    id = source.id;

    return *this;
}

bool Person::setName( char* s ) {
    if(strlen( s ) < 1 )
	return false;
    strncpy( name, s, STRLEN );
    return true;
}

bool Person::setID( int num ) {
    if( num <= 0 )
	return false;

    id = num;
    return true;
}

bool Person::setEmail( char* s ) {
    if( strlen( s ) < 1 )
	return false;
    strncpy( email, s, STRLEN );
    return true;
}

void Person::inputPerson() {
    char tempName[STRLEN+1];
    char tempEmail[STRLEN+1];
    int num;

    do {
	cout << "Enter the person's name: ";
	cin.getline( tempName, STRLEN );
    } while( !setName( tempName ) );
    
    do {
	cout << "Enter the person's email: ";
	cin.getline( tempEmail, STRLEN );
    } while( !setEmail( tempEmail ) );

    do {
	cout << "Enter the person's id: ";
	cin >> num;
	if(cin.fail()) {
	    cin.clear();
	    cin.ignore( 100, '\n' );
	}
    } while(!setID( num ) );
    cin.ignore();
}

void Person::printPerson()const {
    cout << "Name: " << left << setw( STRLEN ) << name;
    cout << "ID: " << right << fixed << id << endl;
    cout << "Email: " << left << setw( STRLEN ) << email;
}
