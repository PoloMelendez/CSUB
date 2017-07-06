#include <iostream>
#include <iomanip>
#include <cstring>
#include "employee.h"
using namespace std;

Employee::Employee()
{
    name[0] = '\0';
    SSN[0] = '\0';
}

Employee::Employee(const Employee &source)
{
    strncpy(name, source.name, NAME_LEN);
    strncpy(SSN, source.SSN, SSN_LEN);
}

Employee &Employee::operator=(const Employee &source)
{
    strncpy(name, source.name, NAME_LEN);
    strncpy(SSN, source.SSN, SSN_LEN);

    return *this;
}

bool Employee::setName(char *s)
{
    if(strlen(s) < 1)
	return false;

    strncpy(name, s, NAME_LEN);
    return true;
}

bool Employee::setSSN(char *s)
{
    if(strlen(s) != 9) return false;

    for(int i = 0; i < 9; i++)
    {
	if(!isdigit(s[i])) return false;
    }

    strcpy(SSN, s);
    return true;
}

void Employee::inputEmployee()
{
    char temp[124];

    do
    {
	cout << "Enter the employee's name: ";
	cin.getline(temp, 124);
    } while(!setName(temp));

    do
    {
	cout << "Enter the employee's 9-digit SSN: ";
	cin.getline(temp, 124);
    } while(!setSSN(temp));
}

void Employee::printEmployee()
{
    cout << "Name: " << setw(NAME_LEN) << left << name;
    cout << "SSN: " << setw(SSN_LEN) << left << SSN << endl;
}







//Hourly Employee
hourlyEmployee::hourlyEmployee(const hourlyEmployee &source) : Employee(source) 
{
    rate = source.rate;
    hours = source.hours;
}

hourlyEmployee &hourlyEmployee::operator=(const hourlyEmployee &source)
{
    Employee::operator=(source);

    rate = source.rate;
    hours = source.hours;

    return *this;
}

bool hourlyEmployee::setRate(double r)
{
    if(r < 8.00) return false;

    rate = r;
    return true;
}

bool hourlyEmployee::setHours(double h)
{
    if(h < 0) return false;

    hours = h;
    return true;
}

void hourlyEmployee::inputEmployee()
{
    double temp;

    Employee::inputEmployee();

    cout << "Enter the employee's pay rate: $";
    cin >> temp;

    while(cin.fail() || !setRate(temp))
    {
	if(cin.fail())
	{
	    cin.clear();
	    cin.ignore(50, '\n');
	}
	cout << "The rate must be at least $8.00. Please enter the rate again: $";
	cin >> temp;
    }

    cout << "Enter the number of hours worked in the pay period: ";
    cin >> temp;

    while(cin.fail() || !setHours(temp))
    {
	if(cin.fail())
	{
	    cin.clear();
	    cin.ignore(50, '\n');
	}
	cout << "Invalid number of hours. Enter the hours worked: ";
	cin >> temp;
    }

    cin.ignore();
}

void hourlyEmployee::printEmployee()
{
    Employee::printEmployee();

    cout << "Employee Type: Hourly\n";
    cout << "Pay Rate: $" << left << setw(10) << rate;
    cout << "Hours Worked: " << hours << endl;
    cout << "Total Pay: $" << (rate * hours) << endl;
}







//Salaried Employee
salariedEmployee::salariedEmployee(const salariedEmployee &source) : Employee(source) 
{
    salary = source.salary;
    weeks = source.weeks;
}

salariedEmployee &salariedEmployee::operator=(const salariedEmployee &source)
{
    Employee::operator=(source);

    salary = source.salary;
    weeks = source.weeks;

    return *this;
}

bool salariedEmployee::setSalary(double s)
{
    if(s <= 0) return false;

    salary = s;
    return true;
}

bool salariedEmployee::setWeeks(int s)
{
    if(s <= 0) return false;

    weeks = s;
    return true;
}

void salariedEmployee::inputEmployee()
{
    double temp;
    int num;

    Employee::inputEmployee();

    cout << "Enter the employee's weekly salary: $";
    cin >> temp;

    while(cin.fail() || !setSalary(temp))
    {
	if(cin.fail())
	{
	    cin.clear();
	    cin.ignore(50, '\n');
	}
	cout << "That salary is invalid. Please enter the salary again: $";
	cin >> temp;
    } 

    cout << "Enter the number of weeks the employee has worked: ";
    cin >> num;

    while(cin.fail() || !setWeeks(num))
    {
	if(cin.fail())
	{
	    cin.clear();
	    cin.ignore(50, '\n');
	}
	cout << "The number of weeks must be positive. Please re-enter: ";
	cin >> num;
    }

    cin.ignore();
}

void salariedEmployee::printEmployee()
{
    Employee::printEmployee();

    cout << "Employee Type: Salaried\n";
    cout << "Weekly Salary: $" << left << setw(10) << salary;
    cout << "Weeks Worked: " << weeks << endl;
    cout << "Total Pay: $" << (salary * weeks) << endl;
}


