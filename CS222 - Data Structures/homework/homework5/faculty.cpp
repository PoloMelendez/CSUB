#include<iostream>
#include<iomanip>
#include<cstring>
#include "faculty.h"
using namespace std;

Faculty::Faculty() : Person() {
    strncpy(department, "", STRLEN+1);
    monthlySalary = 0;
}

Faculty::Faculty( const Faculty &source ) : Person( source ) {
    strncpy( department, source.department, STRLEN );
    monthlySalary = source.monthlySalary;
}

Faculty &Faculty::operator=( const Faculty &source ) {
    Person::operator=(source);

    strncpy(department, source.department, STRLEN );
    monthlySalary = source.monthlySalary;

    return *this;
}

bool Faculty::setDepartment( char *s )
{
    if( strlen( s ) < 1 )
	return false;

    strncpy( department, s, STRLEN );
    return true;
}

bool Faculty::setSalary( double num )
{
    if( num <= 0 )
	return false;

    monthlySalary = num;
    return true;
}

void Faculty::inputPerson()
{
    char temp[STRLEN+1];
    double num;

    Person::inputPerson();

    do
    {
	cout << "Enter the person's department: ";
	cin.getline( temp, STRLEN );
    } while(!setDepartment( temp ));

    do
    {
	cout << "Enter the person's monthly salary: $";
	cin >> num;
	if(cin.fail())
	{
	    cin.clear();
	    cin.ignore( 100, '\n' );
	}
    } while(!setSalary( num ));
    cin.ignore();
}

void Faculty::printPerson() const
{
    Person::printPerson();

    cout << "Department: " << left << setw( STRLEN ) << department;
    cout << "Salary: $" << right << fixed << showpoint << setprecision( 2 ) 
	<< monthlySalary << endl;
}

