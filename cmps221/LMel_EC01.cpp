//Leopoldo Melendez
//Extra Credit 01
//Oct 10, 2013
#include<iostream>
#include<cmath>

using namespace std;

int main()
{
    int input;
    //Asks the user to enter a number to test up to
    //stores the input of the user in an int
    cout<<"Enter the maximum number to be tested: ";
    cin>>input;

    //Declares and initializes a running total and an
    //int called holder to keep of the odd number at the end of the
    //current line
    int holder=3;
    int total=1;

    //Prints an error message if the user inputs an even
    //number, or a negative number
    if(input>1 && input%2 !=0)
    {
   	//Chooses a task to execute based on the input of the user
	//The number of lines printed will be based on the the size
	//of the number chosen	
       	for(int i=1;i<input;i=i+2)
    	{
		cout<<1<<" + "<<3;
		if(holder == 3)
			cout<<"";
		else if(holder<=5 && holder>3)
			cout<<" + "<<5;
		else
		{
		cout<<" + ... + "<<holder;
		}
		total = total + holder;
		cout<<" = "<<total<<" (square root: "<<sqrt(total)<<")"<<endl;
    		holder = holder+2;
    	}
    }	 

    else
	cout<<"The number you select must be both odd and greater then 1"<<endl;


    return 0;
}
