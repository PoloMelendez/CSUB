#include<iostream>

using namespace std;

int base2(int n);

int main() {

    int num;

    do {
	cout << "Enter a positive intger greater than 0 : ";
	cin >> num;
    } while(num <= 0);
    cout << "Your number in base two is : ";
    base2(num);
    cout << endl;

    return 0;
}

int base2(int n) {
    if( n == 1 ) cout << n;
    else {
	base2( n / 2 );
	cout << n % 2;
    }
}
