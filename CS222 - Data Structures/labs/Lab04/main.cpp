#include<iostream>
#include<iomanip>
#include "score.h"

using namespace std;

int main()
{
    Score b;
    double i;

    cout << "Enter a number for 'a': ";
    cin >> i;

    Score a(i);
    cout << "'a' is " << a << endl;

    cout << "\nEnter a number to adjust 'a' by: ";
    cin >> i;

    a += i;

    cout << "After adjust, 'a' is now " << a << endl;

    cout << "\nEnter a number for 'b': ";
    cin >> b;

    Score clone(b);

    cout << "'b' is " << b << endl;
    cout << "The clone of 'b' is " << clone << endl;

    clone = a + b;

    cout << "\nAfter 'clone' = 'a' + 'b', 'clone' is now " << clone << endl;

    cout << "\nTesting subtraction...\n        a is " << a << endl;
    cout << "        b is " << b << endl << "  (a - b) is " << (a - b) << endl;
    cout << "After subtraction, a is " << a << endl;

    cout << "\nTesting comparison operators.\n";
    cout << "'a' is " << a << endl;
    cout << "'b' is " << b << endl;
    cout << endl;

    if(a < b)
	cout << "'a' is less than 'b'.\n";
    else
	cout << "'a' is NOT less than 'b'.\n";

    if(a <= b)
	cout << "'a' is less or equal to 'b'.\n";
    else
	cout << "'a' is NOT less or equal to 'b'.\n";

    if(a > b)
	cout << "'a' is greater than 'b'.\n";
    else
	cout << "'a' is NOT greater than 'b'.\n";

    if(a >= b)
	cout << "'a' is greater than or equal to 'b'.\n";
    else
	cout << "'a' is NOT greater than or equal to 'b'.\n";

    if(a == b)
	cout << "'a' is equal to 'b' (equality operator).\n";
    else
	cout << "'a' is NOT equal to 'b' (equality operator).\n";

    if(a != b)
	cout << "'a' is NOT equal to 'b' (inequality operator).\n";
    else
	cout << "'a' is equal to 'b' (inequality operator).\n";

    return 0;
}

