//Leopoldo Melendez
//Lab 08
//10-22-2013
#include<iostream>
using namespace std;

int UNIT_LENGTH = 0, UNIT_WEIGHT = 1;
double feet = 12;
double meterFeet = 3.28084;
double meterInches = 39.3701;
double kg = 2.20462;
double BMI;

//Prototypes for the functions that will be used, including
//some default values in calc_BMI and convert_units
bool ask_system();
void calc_BMI(double, double, double&, bool=true);
double convert_units(double, int=UNIT_LENGTH);

int main()
{
    double height,  weight;

    //If the user selects y or Y, then the first set of instructions
    //will be used. Otherwise, the second will be used.
    if(ask_system())
    {
	cout<<"Okay, using metric..."<<endl;
	cout<<"What is your height in m? ";
	cin>>height;
	cout<<"What is your weight in kg? ";
	cin>>weight;
	calc_BMI(height, weight, BMI);
        cout<<"Your BMI is "<<BMI<<endl;
    }

    else
    {
	cout<<"Okay, using pounds and inches..."<<endl;
	cout<<"What is your height in inches? ";
	cin>>height;
	cout<<"What is your weight in pounds? ";
	cin>>weight;
	calc_BMI(height, weight, BMI, false);
	cout<<"Your BMI is "<<BMI<<endl;
    }

    return 0;
}

//Asks the user for an input and returns a true or false based
//on that input. Will also repeatedly ask for a new input if an
//invalid input is given by the user.
bool ask_system()
{
    char input;
    bool choice;
    cout<<"Do you want to use metric measurements (Y/N)? ";
    cin>>input;
    while(input!='y' && input!='Y' && input!='n' && input!='N')
    {
	cout<<"Error: Improper Input"<<endl;
	cout<<"Do you want to use metric measurements (Y/N)? ";
	cin>>input;
    }
    if(input == 'y' || input == 'Y')
	choice = true;
    else if(input == 'n' || input == 'N')
	choice = false;
    return choice;
}

//Calls the convert_units function depending on if the user chose metric
//or standard. Takes the values given and calculates the BMI.
void calc_BMI(double height, double weight, double &BMI, bool is_metric)
{
    if(is_metric)
	BMI = weight/(height*height);
    else
    {
	height = convert_units(height);
	weight = convert_units(weight, UNIT_WEIGHT);
	BMI = weight/(height*height);
    }
}

//If the user has selected standard measurement, will convert the units
//into metric so the calc_BMI function will have an accurate result.
double convert_units(double value, int unit_type)
{
    if(unit_type == UNIT_WEIGHT)
	value = value/kg;
    else
	value = value/meterInches;
    return value;
}
