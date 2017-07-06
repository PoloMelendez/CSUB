//Leopoldo Melendez
//HW 03
//10 - 1 - 2013
#include<iostream>
#include<string>

using namespace std;
int main()
{
    double number1;
    double number2;
    char operation;
    
    do{
	//Asks the user for a couple of doubles as inputs, as well as to choose an operator
	//Will loop through continuously until the user selects q or Q to quit
	cout<<"Enter a double number: ";
	cin>>number1;
	cout<<"Enter a second double number: ";
	cin>>number2;
	
	//Prints out a menu of operators
	cout<<"Enter the letter corresponding to your desired operation\na(add)  s(subtract)  m(multiply)  d(divide)  q(quit): ";
	cin>>operation;
	
	//Chooses which case corresponds with the users input and performs that case's corresponding operation
	//Then, prints out the solution, and continues the loop, unless the user has chosen q or Q
	switch(operation){
		case 's' :
		case 'S' :    
    			cout<<number1<<" - "<<number2<<" = "<<number1-number2<<endl;
			break;
		case 'a' :
		case 'A' :	
			cout<<number1<<" + "<<number2<<" = "<<number1+number2<<endl;
			break;
		case 'd' :
		case 'D' :
	        	cout<<number1<<" / "<<number2<<" = "<<number1/number2<<endl;
	        	break;
		case 'm' :
		case 'M' :
	        	cout<<number1<<" * "<<number2<<" = "<<number1*number2<<endl;
	        	break;
		case 'q' :
		case 'Q' :
			break;
		default :
			cout<<"Invalid operator"<<endl;	
			break;
    		}

    	}while(!((operation=='q')||(operation=='Q')));
    return 0;
}
