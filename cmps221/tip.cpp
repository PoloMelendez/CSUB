#include<iostream>
using namespace std;
int main()
{
    double billAmount;
    double tipPercent = 0.15;

    cout<<"Enter your bill amount: ";
    cin>>billAmount;

    double tipTotal= billAmount*tipPercent;

    cout<<"The tip is: "<<tipTotal<<" dollars"<<endl;

    double billTotal = billAmount+tipTotal;

    cout<<"The total bill including tip is: "<<billTotal<<" dollars"<<endl;


    return 0;
}
