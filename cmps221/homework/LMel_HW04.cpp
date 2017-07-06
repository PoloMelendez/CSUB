//Leopoldo Melendez
//HW 04
//10 - 15 - 2013
#include<iostream>
using namespace std;

void show_ordinal(int);
void show_verse(int, int);
void show_num_text(int);
void show_ordinal_text(int);

int main()
{

    //Outputs a menu for the user to choose between numbers or descriptive words
    //in the verses.
    int choice;
    cout<<"Choose either numeric or descriptive words to be used in the verses: "<<endl;
    cout<<"Enter 1 for numeric, or 2 for descriptive: ";
    cin>>choice;
    while(choice != 1 && choice != 2)
    {
      	cout<<"Incorrect input, please try again";
        cin>>choice;
    }
    cout<<endl;

    //Calls the show_verse 12 times to output all 12 verses.
    for(int i=1;i<=12;i++)
    {
	cout<<"On the ";
	if(choice==1)
	    show_ordinal(i);
	else if(choice==2)
	    show_ordinal_text(i);
	cout<<" day of Christmas,"<<endl<<"My true love gave to me,"<<endl;
	show_verse(i, choice);
    }


    return 0;
}

//Function that chooses which ordinal suffix to add onto the numbers
void show_ordinal(int number)
{
    if(number==11 || number==12 || number==13)
	cout<<number<<"th";
    else
    {
    	switch(number%10)
    	{
	    case 1 :
		cout<<number<<"st";
	        break;
	    case 2 :
	        cout<<number<<"nd";
	        break;
            case 3 :
 	        cout<<number<<"rd";
	        break;
	    default :
	        cout<<number<<"th";
	        break;
	}
    }
}

//function that changes numbers to text for when the user selects descriptive
//words for the verses
void show_num_text(int number)
{
    switch(number)
    {
	case 1:
	    cout<<"one";
	    break;
	case 2:
	    cout<<"two";
	    break;
	case 3:
	    cout<<"three";
	    break;
	case 4:
	    cout<<"four";
	    break;
	case 5:
	    cout<<"five";
	    break;
	case 6:
	    cout<<"six";
	    break;
	case 7:
	    cout<<"seven";
	    break;
	case 8:
	    cout<<"eight";
	    break;
	case 9:
	    cout<<"nine";
	    break;
	case 10:
	    cout<<"ten";
	    break;
	case 11:
	    cout<<"eleven";
	    break;
	case 12:
	    cout<<"twelve";
	    break;
    }
}    

//Function that uses a switch to decide what line of a verse is to be output
//and loops through to complete one full verse
void show_verse(int verse, int choice)
{	
if(verse == 1)
	cout<<"A partridge in a pear tree."<<endl<<endl;
else
{
    for(verse;verse>=1;verse--)
    {
    	switch(verse)
    	{
	    case 12 :
		if(choice==1)
		    cout<<"12 drummers drumming,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" drummers drumming,"<<endl;
		}
	        break;
	    case 11 :
	        if(choice==1)
		    cout<<"11 pipers piping,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" pipers piping,"<<endl;
		} 
	       	break;
	    case 10 :
	        if(choice==1)
		cout<<"10 lords-a-leaping,"<<endl;
		else if(choice==2)
                {
		    show_num_text(verse);
                    cout<<" lords-a-leaping,"<<endl;
		}		    
	        break;
	    case 9 :
	        if(choice==1)
		    cout<<"9 ladies dancing,"<<endl;
		else if(choice==2)
                {
	 	    show_num_text(verse);
		    cout<<" ladies dancing,"<<endl;
		}
		break;
	    case 8 :
	        if(choice==1)
		    cout<<"8 maids-a-milking,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" maids-a-milking,"<<endl;
		break;
	    case 7 :
	        if(choice==1)
		cout<<"7 swans-a-swimming,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" swans-a-swimming,"<<endl;
		}
		break;
	    case 6 :
	        if(choice==1)
		cout<<"6 geese-a-laying,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" geese-a-laying,"<<endl;
		}
		break;
	    case 5 :
	        if(choice==1)
		cout<<"5 golden rings,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" golden rings,"<<endl;
		}
		break;
	    case 4 :
	        if(choice==1)
		cout<<"4 colly birds,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" colly birds,"<<endl;
		}
		break;
	    case 3 :
	        if(choice==1)
		cout<<"3 french hens,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" french hens,"<<endl;
		}
		break;
	    case 2 :
	        if(choice==1)
		cout<<"2 turtle doves,"<<endl;
		else if(choice==2)
		{
		    show_num_text(verse);
		    cout<<" turtle doves,"<<endl;
		}
		break;
	    case 1:
	        cout<<"And a partridge in a pear tree."<<endl<<endl;
	        break;
	}
    }
}
}
}

//Function that changes numbers to words with the correct suffixes applied
void show_ordinal_text(int number)
{
    switch(number)
    {
	case 1:
	    cout<<"first";
	    break;
	case 2:
	    cout<<"second";
	    break;
	case 3:
	    cout<<"third";
	    break;
	case 4:
	    cout<<"fourth";
	    break;
	case 5:
	    cout<<"fifth";
	    break;
	case 6:
	    cout<<"sixth";
	    break;
	case 7:
	    cout<<"seventh";
	    break;
	case 8:
	    cout<<"eighth";
	    break;
	case 9:
	    cout<<"ninth";
	    break;
	case 10:
	    cout<<"tenth";
	    break;
	case 11:
	    cout<<"eleventh";
	    break;
	case 12:
	    cout<<"twelfth";
	    break;
    }
}
