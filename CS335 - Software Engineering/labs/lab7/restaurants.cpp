using namespace std;
#include "restaurants.h"

void restaurants::run()
{
    string input;
    LOGFILE<<"Starting run function" << endl;
    destination = "nowhere";

    cout << "Are you in Canada?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	destination = "Time Hortons";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Is it before 10:30AM?" << endl;
    getline(cin, input);
    if(input == "yes" ) {
	cout << "Do you want donuts?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Dunkin Donuts";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "McDonalds";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you live on the west coast?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	cout << "Is it after 10:00PM?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Jack in the Box";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "In-n-Out";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want a $5 footlong?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	destination = "Subway";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you have less than five bucks?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	destination = "Taco Bell";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want pizza?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	cout << "Do you want garlic crust?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Dominos";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "Pizza Hut";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want a classic hamburger?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	destination = "White Castle";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want dessert?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	cout << "Are you in your car?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Sonic";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "Dairy Queen";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want fried food?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	cout << "Do you want fish?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Long John Silvers";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "Kentucky Fried Chicken";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Do you want Arbys?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	destination = "Arbys";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    cout << "Is it Sunday?" << endl;
    getline(cin, input);
    if( input == "yes" ) {
	cout << "Do you want a square patty?" << endl;
	getline(cin, input);
	if( input == "yes" ) {
	    destination = "Wendy's";
	    LOGFILE<<"Exiting run function final destination was " << destination << endl;
	    return;
	}
	destination = "Burger King";
	LOGFILE<<"Exiting run function final destination was " << destination << endl;
	return;
    }

    destination = "Chick-fil-a";
    LOGFILE<<"Exiting run function final destination was " << destination << endl;
}

string restaurants::selection()
{
    LOGFILE<<"Starting selection function" << endl;
    LOGFILE<<"Exiting the selection function and returning " << 
	destination << endl;
    return destination;
}
