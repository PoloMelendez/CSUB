//Leopoldo Melendez
//Lab 05a
//10-3-2013
#include<iostream>
using namespace std;
int main()
{
    float ENTREE = 1.99, DRINK = 0.89, DESSERT = 2.99;
    int entreeNumber=0, drinkNumber=0, dessertNumber=0;
    int input=0;

    cout<<"What would you like to do?\n 1) Add an entree\n 2) Add a drink\n 3) Add a dessert\n 4) Complete my order"<<endl;
    
    do{
	cout<<"Enter selection: ";
    	cin>>input;
	char plural;

    	switch(input){
		case 1 :
		    if(entreeNumber>=1)
			plural='s';
		    else
			plural;
		    entreeNumber++;
		    cout<<"You now have "<<entreeNumber<<" entree"<<plural<<" in your tray."<<endl;
		    break;
		case 2 :
		    drinkNumber++;
		    cout<<"You now have "<<drinkNumber<<" drink in your tray."<<endl;
		    break;
		case 3 :
		    dessertNumber++;
		    cout<<"You now have "<<dessertNumber<<" dessert in your tray."<<endl;
		    break;
		case 4 :
                    cout<<"Your order is complete. Please pay $"<<entreeNumber*ENTREE+drinkNumber*DRINK+dessertNumber*DESSERT<<endl;
		    break;
		default :
		    cout<<"Invalid selection"<<endl;
		    break;
	}
    }while(input!=4);	



    return 0;
}
