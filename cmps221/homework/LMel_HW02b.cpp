//Leopoldo Melendez
//HW02b
//Sep 26, 2013
#include<iostream>
#include<iomanip>
using namespace std;
int main()
{   
    double baseHourlyRate = -1;
    double baseHoursWorked = -1;
    double overtimeHoursWorked = -1;

    cout<<"Enter the Base Hourly Rate: ";
    while(baseHourlyRate<0)
   	 cin>>baseHourlyRate;
    cout<<endl<<"Enter the Base Hours Worked: ";
    while(baseHoursWorked<0)
	cin>>baseHoursWorked;
    cout<<endl<<"Enter the Overtime Hours Worked: ";
    while(overtimeHoursWorked<0)
	cin>>overtimeHoursWorked;

    const float OvertimeRate=1.5*baseHourlyRate;

    double overTime = overtimeHoursWorked*OvertimeRate;
    double wages = baseHourlyRate*baseHoursWorked;
    double grossWages = wages+overTime;
    
    const float IncomeTaxRate = 0.15;

    double taxes;
    
    if(grossWages<=1000)
   	taxes = grossWages*IncomeTaxRate;
    else
	taxes = grossWages*0.25;
    
    double netWages = grossWages - taxes;

    cout<<"Base Rate	Hours Worked	O/T Hours	Gross Wages	Taxes	Net Wages"<<endl;
    cout<<baseHourlyRate<<"		"<<baseHoursWorked<<"		"<<overtimeHoursWorked
	<<"		"<<setprecision(2) << fixed <<grossWages<<"		"<<taxes<<"	"<<netWages<<endl;




    return 0;
}
