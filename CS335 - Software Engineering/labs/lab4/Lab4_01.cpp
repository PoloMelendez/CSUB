//An Age Old Question
//Problem #38
//Added by manfred-burmeister
//My chosen coding style was K&R
//
//Leopoldo Melendez
//
//The problem turned out to be much more simple than I originally
//thought it would be, so I came up with the function for finding
//the least common multiples in general, instead of just using 60. Basically,
//the son's age is found easily using nested for loops and % operators,
//and that is added to the least common multiple of the numbers for the
//grandfather's age.
#include<iostream>

using namespace std;

int leastcf(int a, int b);
int gcd(int a, int b);

int main()
{
    int num = 0;
    while(true){
	if (num%3 == 1) {
	    if (num%4 == 2) {
		if (num%5 == 4) {
		    break;
		}
	    }
	}
	num++;
    }

    int lcommonf = leastcf(3, leastcf(4, 5));
    cout << "The grandfather is " << lcommonf + num << " years old" << endl;

    return 0;
}

int leastcf(int a, int b)
{
    int num = (a*b)/gcd(a,b);
    return num;
}

int gcd(int a, int b)
{
    if (b == 0)
	return a;
    else
	return gcd(b, a%b);
}
