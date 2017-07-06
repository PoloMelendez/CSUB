#include<iostream>

using namespace std;

int callCount = 0;
int fib( int );
int fib2( int );

int main()
{
    int n;
    int f1 =0, f2 = 1, f3 = 1;

    while( true ) {
	cout << "Enter an integer: ";
	cin >> n;

	cout<< "The " << n << "th fibonacci number is " << fib2(n) << endl;
	cout<< "The number of function fibi() called: " << callCount << endl;
	callCount = 0;
    }
    return 0;
}

int fib2( int n ){
    int f1 = 0, f2 = 1, f3 = 1;
    callCount++;
    for( int i = 3; i <= n; i++ ) {
	f1 = f2;
	f2 = f3;
	f3 = f1 + f2;
    }
    return f3;
}

int fib( int n ){
}
