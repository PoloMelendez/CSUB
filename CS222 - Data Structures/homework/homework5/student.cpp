#include<iostream>
#include<iomanip>
#include<cstring>
#include "student.h"
using namespace std;

Student::Student() : Person() {
    strncpy( major, "", STRLEN+1 );
    units = 0;
    grades = 0;
}

Student::Student( const Student &source ) : Person( source ) {
    strncpy( major, source.major, STRLEN );
    units = source.units;
    grades = source.grades;
}

Student &Student::operator=( const Student &source ) {
    Person::operator=(source);

    strncpy(major, source.major, STRLEN);
    units = source.units;
    grades = source.grades;

    return *this;
}

bool Student::setUnits( double num ) {
    if( num <= 0 )
	return false;

    units = num;
    return true;
}

bool Student::setWeightedGrades( double num ) {
    if( num <= 0 )
	return false;

    grades = num;
    return true;
}

bool Student::setMajor( char* s ) { 
    if( strlen( s ) < 1 )
	return false;

    strncpy( major, s, STRLEN );
    return true;
}

double Student::calcGPA()const {
    if( units == 0 )
	return 0;
    return grades/units;
}

void Student::inputPerson() {
    char temp[STRLEN+1];
    double num1, num2;

    Person::inputPerson();

    do{
	cout << "Enter the person's major: ";
	cin.getline( temp, STRLEN );
    } while(!setMajor( temp ));

    do {
	cout << "Enter the person's grades: ";
	cin >> num1;
	if(cin.fail()) {
	    cin.clear();
	    cin.ignore( 100, '\n' );
	}
    } while(!setWeightedGrades( num1 ) );
    cin.ignore();

    do {
	cout << "Enter the person's units: ";
	cin >> num2;
	if(cin.fail()) {
	    cin.clear();
	    cin.ignore( 100, '\n' );
	}
    } while(!setUnits( num2 ) );
    cin.ignore();
}

void Student::printPerson()const {
    Person::printPerson();

    cout << "Major: " << left << setw( STRLEN ) << major;
    cout << "Grades: " << right << fixed << showpoint << setprecision( 2 )
	<< grades << endl;
    cout << "Units: " << right << fixed << showpoint << setprecision( 2 )
	<< units << endl;
}
