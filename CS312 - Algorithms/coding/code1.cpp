//code1 cs312   Polo Melendez
#include <iostream>
#include<cmath>

using namespace std;

const double error = 0.000001;
const double start = 0.00001;
const double MAX = 999999999;

int main()
{
    double n1, n2;
    cout << "Input 2 numbers: ";
    cin >> n1 >> n2;
    double min = start;
    double max = 1.0;
    if(n2 < 1 && n2 > 0) {
        min = 1;
        max = MAX;
    }
    double mult = start;
    double diff = 1 - (n2 * mult);
    while( fabs(diff) > error) {
        if( diff < 0.0 )
            max = mult;
        else
            min = mult;
        mult = (min + max) * 0.5;
        diff = 1.0 - (n2 * mult);
    }
    cout << "Answer is: " << n1 * mult << endl;

    return 0;
}
