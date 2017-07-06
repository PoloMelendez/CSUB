//Leopoldo Melendez
//Lab or HW #
//fibonacci numbers summed
#include<iostream>
using namespace std;
int main()
{
    int previous1 = 1;
    int previous2 = 0;
    int sum = 0;
    for(int current = 1;current<=4000000;current = previous1 + previous2)
    {
	previous2 = previous1;
	previous1 = current;
	if(current%2 == 0)
	    sum = sum + current;
	if(sum%100000000000000 < 10 && sum%100000000000000 > 0) {
	    cout << sum;
	    break;
	}
    }
    cout<<sum<<endl;


    return 0;
}
