#include<iostream>
#include<iomanip>

using namespace std;

int main(){
    int *p1;
    int	*p2;
    int v1 = 50, v2 = 40;

    cout << "p1 is " << p1 << endl;
    cout << "p2 is " << p2 << endl;
    cout << "v1 is " << v1 << endl;
    cout << "v1's address is " << &v1 << endl;
    cout << "v2 is " << v2 << endl;
    cout << "v2's address is " << &v2 << endl << endl;
    


    p1 = &v1;
    cout << "a. The address p1 points to is " << p1 << endl << endl;

    p2 = p1;

    cout << "b. p2 now points to the same address as p1, so p2 is " << p2 << endl << endl;

    *p2 = 99;

    cout << "c. The value within the address pointed to by p2 is now " << *p2 << endl << endl;

    p2 = &v2;

    cout << "d. p2 now points at the address at v2, p2 is now " << p2 << endl << endl;

    cout << "e. p1 = " << p1 << endl << endl;
    cout << "f. *p2 = " << *p2 << endl << endl;
    cout << "g. &vi = " << &v1 << endl << endl;


    return 0;
}
