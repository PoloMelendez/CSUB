//Leopoldo Melendez
//Lab or HW #
//Date
#include<iostream>
using namespace std;
int main()
{
    double baseDamage=0;
    double critPercent=0;    
    double attacksPerSecond = 1;
    double totalSeconds = 10;

    cout<<"Enter the current base damage of your hero: ";
    cin>>baseDamage;
    cout<<"Enter the current percent chance to crit of your hero: ";
    cin>>critPercent;
    cout<<endl;

    double critChance = critPercent/100;
    double totalDamage = baseDamage*attacksPerSecond*totalSeconds*(1 + critChance);
    
    cout<<"Your total damage over "<<totalSeconds<<"seconds would be "<<totalDamage<<endl<<endl;
    
    int input = 0;

    while(input != 10)
    {
	cout<<"1: Compare two damage items	2:etc	10: Quit "<<endl;
	cout<<"Choose a task: ";
	cin>>input;

	double aC1;
	double aC2;
	double aD1;
	double aD2;
	double cC1;
	double cC2;
	double tD1;
	double tD2;

	switch(input)
	{
	    case 1 :
		cout<<"Enter the added damage provided by the first item: ";
		cin>>aD1;
		cout<<"Enter the added crit percent provided by the first item: ";
		cin>>aC1;
		cout<<endl;
		cC1 = critChance + aC1/100;
		tD1 = (baseDamage + aD1)*attacksPerSecond*totalSeconds*(1 + critChance + cC1); 
		cout<<"Over 10 seconds you would do approximately "<<tD1<<" damage with the first item"<<endl<<endl;
		cout<<"Enter the added damage provided by the second item: ";
		cin>>aD2;
		cout<<"Enter the added crit percent provided by the second item: ";
		cin>>aC2;
		cout<<endl;
		cC2 = critChance + aC2/100;
		tD2 = (baseDamage + aD2)*attacksPerSecond*totalSeconds*(1 + critChance + cC2);
		cout<<"Over 10 seconds you would do approximately "<<tD2<<" damage with the second item"<<endl<<endl;
		break;
	    case 10 : 
		break;	
	    default :
		break;
	}
    }
    return 0;
}
