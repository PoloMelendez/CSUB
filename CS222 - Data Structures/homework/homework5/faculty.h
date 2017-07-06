#ifndef FACULTY_H
#define FACULTY_H

#include<iostream>
#include "person.h"

using namespace std;

class Faculty : Person {
    private:
	char department[STRLEN+1];
	double monthlySalary;

    public:
	Faculty();
	Faculty( const Faculty & );

	Faculty &operator=( const Faculty & );

	bool setDepartment( char * );
	bool setSalary( double );

	const char *getDepartment() const { return department; }
	double getSalary() const { return monthlySalary; }

	void inputPerson();
	void printPerson()const;
};
#endif
