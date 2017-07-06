#include <iostream>
#include "student.h"
#include "faculty.h"
using namespace std;

int main()
{
    Student stu;
    Faculty anon;

    cout << "Testing the child class objects....\n";

    cout << "\nInput the Student details.\n";
    stu.inputPerson();

    cout << "\nInput the Faculty details.\n";
    anon.inputPerson();

    cout << "\nTesting the print functions...\n";

    cout << "\nStudent object:\n";
    stu.printPerson();

    cout << "\nFaculty object:\n";
    anon.printPerson();

    return 0;
}

