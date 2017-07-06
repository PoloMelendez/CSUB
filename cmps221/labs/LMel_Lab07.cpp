//Leopoldo Melendez
//Lab 07
//10-17-2013
#include<iostream>
using namespace std;

int smallest_int;
int largest_int;
double smallest_double;
double largest_double;
int num_ints;
int num_doubles;
double running_total;
double average;

int get_num();
int get_num(int);
int get_num(int, int);
double get_num(double);
double get_num(double, double);
void get_numbers(int, int, int);
void get_numbers(int, double, double);
double calc_avg(int);
void show_results();

int main()
{    
    //Asks the user to choose the number of inputs, the minimum int, the maximum int,
    //the minimum double, the maximum double, a number of ints and doubles equal to
    //the number of inputs they chose, and then outputs the average
    cout<<"How many numbers to enter? ";
    
    num_ints = get_num(2);
    num_doubles = num_ints;

    cout<<"Enter the smallest allowed integer: ";
    smallest_int = get_num();
    cout<<"Enter the largest allowed integer: ";
    largest_int = get_num(smallest_int);

    get_numbers(num_ints, smallest_int, largest_int);

    cout<<"Enter the smallest allowed double: ";
    cin>>smallest_double;
    cout<<"Enter the largest allowed double: ";
    largest_double = get_num(smallest_double);

    get_numbers(num_doubles, smallest_double, largest_double);

    calc_avg(num_ints + num_doubles);

    show_results();

    return 0;
}

//Accepts an input from the user and returns it
int get_num()
{
    int number;
    cin>>number;
    return number;
}

//Accepts an input from the user and checks to make sure it is greater
//then the minimum value, and returns it
int get_num(int min)
{
    int number;
    cin>>number;
    while(number<min)
    {
	cout<<"Error: number must be at least "<<min<<endl;
	cout<<"Enter integer: ";
	cin>>number;
    }
    cout<<endl;
	return number;
}

//Accepts an input from the user and checks to make sure it is greater
//then the minimum int and less then the maximum double, and returns it
int get_num(int min, int max)
{
    int number;
    cout<<"Enter integer: ";
    cin>>number;
    while(number<min || number>max)
    {
	cout<<"Error: number must be at least "<<min<<" and at most "<<max<<endl;
        cout<<"Enter integer: ";
	cin>>number;
    }
    	return number;
}

//Accepts a double from the user and checks to make sure it is
//greater then the minimum double, and returns it
double get_num(double min)
{
    double number;
    cin>>number;
    while(number<min)
    {
	cout<<"Error: number must be at least "<<min<<endl;
	cout<<"Enter double: ";
	cin>>number;
    }
    cout<<endl;
    	return number;
}

//Accepts a double from the user and checks to make sure it is
//greater then the minimum double and less then the maximum double,
//and returns it
double get_num(double min, double max)
{
    double number;
    cout<<"Enter double: ";
    cin>>number;
    while(number<min || number>max)
    {
	cout<<"Error: number must be at least "<<min<<" and at most "<<max<<endl;
	cout<<"Enter double: ";
	cin>>number;
    }
    	return number;
}

//Updates the running total with each integer entered
void get_numbers(int count, int min, int max)
{
    for(int i=1;i<=count;i++)
    {
	running_total=running_total + get_num(min, max);
    }
    cout<<endl;
}

//Updates the running total with each double entered
void get_numbers(int count, double min, double max)
{
    for(int i=1;i<=count;i++)
    {
	running_total=running_total + get_num(min, max);
    }
    cout<<endl;
}

//Calculates the average of all of the doubles and integers added to
//the running total.
double calc_avg(int count)
{
    double avg = running_total/count;
    return avg;
}

//Outputs the average to the user
void show_results()
{
    cout<<"The average of all the numbers is "<<calc_avg(num_ints + num_doubles)<<endl;
}

