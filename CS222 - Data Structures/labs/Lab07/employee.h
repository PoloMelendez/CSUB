#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include<iostream>
#include <cstring>
using namespace std;

#define NAME_LEN 51
#define SSN_LEN  10

class Employee
{
    private:
	char name[NAME_LEN];
	char SSN[SSN_LEN];

    public:
	Employee();

	Employee(const Employee &);
	Employee &operator=(const Employee &);

	virtual ~Employee() { cout << "***Destructor Employee ***\n"; }

	bool setName(char *s);
	bool setSSN(char *);
	const char *getName() const { return name;     }
	const char *getSSN() const  { return SSN;      }

	virtual void inputEmployee();

	virtual void printEmployee();
};

class hourlyEmployee : public Employee
{
    private:
	double rate;
	double hours;

    public:
	hourlyEmployee() : Employee(), rate(0), hours(0) {}

	hourlyEmployee(const hourlyEmployee&);
	hourlyEmployee &operator=(const hourlyEmployee &);

	virtual ~hourlyEmployee() { cout << "***Destructor Hourly Employee ***\n"; }

	bool setRate(double);
	bool setHours(double);

	double getRate()  const { return rate;  }
	double getHours() const { return hours; }

	virtual void inputEmployee();
	virtual void printEmployee();
};

class salariedEmployee : public Employee
{
    private:
	double salary;
	int weeks;

    public:
	salariedEmployee() : Employee(), salary(0), weeks(0) {}

	salariedEmployee(const salariedEmployee&);
	salariedEmployee &operator=(const salariedEmployee &);

	virtual ~salariedEmployee() { cout << "***Destructor Salaried Employee ***\n"; }

	bool setSalary(double);
	bool setWeeks(int);

	double getSalary() const { return salary; }
	int getWeeks()     const { return weeks;  }

	virtual void inputEmployee();
	virtual void printEmployee();
};

#endif
