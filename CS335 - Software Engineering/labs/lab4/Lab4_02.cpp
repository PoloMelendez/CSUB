//Elevator Directions
//Problem #33
//I chose the K&R coding style
//
//Leopoldo Melendez
//
//The problem ended up being as simple as reading from the txt file and updating the
//floor number. I added limits just in case faulty directions were given to exceed
//the top or bottom of the building, but the given txt file doesn't attempt to do that.
#include<iostream>
#include<fstream>

using namespace std;

int navigate(int);

int main()
{
    int floor = navigate( 1 );

    if ( floor != -20 )
	cout << "The elevator ended on floor number " << floor << endl;

    return 0;
}

int navigate(int floor)
{
    int temp;

    ifstream myfile("floors.txt");
    if (!myfile) {
	return -20 ;
    } while (myfile) {
	myfile >> temp;
	floor += temp;
	if (floor > 100 || floor < -10) {
	    cout << "The elevator cannot go to floor " << floor + temp << endl;
	    return -20;
	}
    }
    return floor;
}
