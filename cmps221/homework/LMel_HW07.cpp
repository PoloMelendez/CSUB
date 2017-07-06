//Leopoldo Melendez
//HW 07
//11-12-13
#include<iostream>
#include<cstdlib>
#include<time.h>
using namespace std;

//Structure that holds an int for suits, int for value, and a bool to determine if the structure has been
//previously selected
struct card
{
    int suit;
    int value;
    bool selected;
};

int main()
{
    //Creates an array of card structures and fills them like a deck of cards, with suit and value
    card deck[52];

    for(int i=0;i<52;i++)
    {
	deck[i].value = (i+1) % 13;
	if(deck[i].value == 0)
	    deck[i].value = 13;


	if(i<=12)
	    deck[i].suit = 1;
	else if(i>=13 && i<=25)
	    deck[i].suit = 2;
	else if(i>=26 && i<=38)
	    deck[i].suit = 3;
	else if(i>=39 && i<52)
	    deck[i].suit = 4;

	deck[i].selected = false;
    }

    //Creates another smaller array of structures to hold the randomly selected cards from the
    //deck array. Uses a switch to transfer the ints into descriptive words, and ouputs the
    //contents of the selections array to the user as 5 randomly selected cards
    card selections[5];

    srand(time(NULL));

    for(int i=0;i<5;i++)
    {
	int randomSelection = rand() % 52 + 1;
	while(deck[randomSelection].selected==true)
	{
	    randomSelection = rand() % 52 + 1;
	}
	selections[i].suit = deck[randomSelection].suit;
	selections[i].value = deck[randomSelection].value;
	deck[randomSelection].selected = true;
    }

    for(int i=0;i<5;i++)
    {
	string selectedSuit;
	string cardSize;
	switch(selections[i].value)
	{
	    case 1:
		cardSize = "Ace";
		cout<<cardSize<<" of ";
		break;
	    case 11:
		cardSize = "Jack";
		cout<<cardSize<<" of ";
		break;
	    case 12:
		cardSize = "Queen";
		cout<<cardSize<<" of ";
		break;
	    case 13:
		cardSize = "King";
		cout<<cardSize<<" of ";
		break;
	    default:
		cout<<selections[i].value<<" of ";
	}
	switch(selections[i].suit)
	{
	    case 1:
		selectedSuit = "Clubs";
		break;
	    case 2:
		selectedSuit = "Diamonds";
		break;
	    case 3:
		selectedSuit = "Hearts";
		break;
	    case 4:
		selectedSuit = "Spades";
		break;
	}
	cout<<selectedSuit<<endl;
    }

    return 0;
}
