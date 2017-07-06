//Leopoldo Melendez
//Lab 04
//10 - 1 - 2013
#include<iostream>
using namespace std;
int main()
{
    //calling and initializing necessary variables.
    int days;
    double pay = 0.01;
    double total = 0.01;
    //prints out message to user and accepts input into days
    cout<<"How many days did you work? ";
    cin>>days;
    //doubles the pay amount through each iteration of the loop and adds
    //to total.
    for(int i=1;i<days;i++)
    {
	pay = pay*2;
	total = total + pay;
    }
    //prints out the total earned after the number of days given
    cout<<"In "<<days<<" day(s), you earned $"<<total<<endl;

    return 0;
}
