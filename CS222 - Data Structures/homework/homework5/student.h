#ifndef STUDENT_H
#define STUDENT_H

#include<iostream>
#include "person.h"

using namespace std;

class Student : public Person {
    private:
	double units;
	double grades;
	char major[STRLEN + 1];

    public:
	Student();
	Student( const Student & );

	Student &operator=( const Student & );

	bool setUnits( double );
	bool setWeightedGrades( double );
	bool setMajor( char * );

	double getUnits() const { return units; }
	double getWeightedGrades() const { return grades; }
	const char* getMajor() const { return major; }

	double calcGPA() const;
	void inputPerson();
	void printPerson() const;
};
#endif
