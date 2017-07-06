#include<iostream>
#include "employee.h"

using namespace std;

int main() {

    Employee * ptr = NULL;
    char cmd[129];

    while( true ) {
	cout << "Enter an 'employee', an 'hourly employee',"
	    << "or 'salaried employee'\nEnter 'exit' to quit: ";
	cin.getline(cmd, 129);

	if(strncmp(cmd, "hourly", 6) == 0 ) {
	    try{
		ptr = new hourlyEmployee;
	    } catch(bad_alloc ex) {
		cout << "Exception: " << ex.what() << endl;
		ptr = NULL;
		return 0;
	    }
	}
	else if(strncmp(cmd, "salaried", 8) == 0) {
	    try{
		ptr = new Employee;
	    } catch(bad_alloc ex) {
		cout << "Exception: " << ex.what() << endl;
		ptr = NULL;
		return 0;
	    }
	}
	else if(strncmp(cmd, "employee", 8) == 0) {
	    try{
		ptr = new Employee;
	    } catch(bad_alloc ex) {
		cout << "Exception: " << ex.what() << endl;
		ptr = NULL;
		return 0;
	    }
	}
	else if(strncmp(cmd, "exit", 4) == 0 || strncmp(cmd, "quit", 4) == 0)
	   break; 
	else {
	    cout << "Invalid Employee Type" << endl;
	    continue;
	}
	ptr->inputEmployee();
	ptr->printEmployee();
	delete ptr;
	ptr = NULL;
    }


    return 0;
}

