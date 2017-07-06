//Leopoldo Melendez
//Lab 12
//11-19-13
#include<iostream>
#include<cstdlib>
using namespace std;

double calc_sum(int, double []);

int main()
{
    //Asks the user to decide how many numbers to enter, then enter that many numbers.
    //Fills an array with those values using pointers.
    int count;

    cout<<"How many numbers to be entered? ";
    cin>>count;

    double array[count];

    for(int i=0;i<count;i++)
    {
	cout<<"Enter number "<<i+1<<": ";
	cin>>*(array + i);
    }

    //Calls the calc_sum function to add up the elements of the array and outputs
    //the value to the user.
    cout<<endl<<"The total of all numbers is "<<calc_sum(count, array)<<endl<<endl;

    //Goes through each element of the array and doubles it using pointers, then calls
    //the calc_sum function a second time and outputs the sum of the doubled values.
    for(int i=0;i<count;i++)
    {
	*(array+i) = *(array+i) * 2;
    }

    cout<<"After doubling each number, the total is now "<<calc_sum(count, array)<<endl;

    return 0;
}

//Function that goes through the given array and adds them together, returning
//that sum.
double calc_sum(int size, double array[])
{
    double sum;
    for(int i=0;i<size;i++)
    {
	sum = sum + *(array + i);
    }
    return sum;
}

