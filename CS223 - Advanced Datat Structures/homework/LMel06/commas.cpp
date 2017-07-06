#include<iostream>
#include<iomanip>
using namespace std;

void commas(int n) {
   //f( n <= 0 ) return;
    
    if(n <=0) return;
    else if( n <= 999 )
	cout << n;
    else {
	commas(n/1000);
	cout<<",";
	if(n%1000 > 0)
	    cout<<n%1000;
	else cout <<"000";
    }
    return;
}

int main() {
    int num;
    cout << "Enter a positive integer: ";
    cin >> num;

    if( num == 0 )
	cout << num << endl;
    else if( num == 1000 )
	cout << num << endl;
    else {
    commas( num );
    cout << endl;
    }

    return 0;
}
