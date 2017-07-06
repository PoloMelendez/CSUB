//Leopoldo Melendez
//Lab 06
//10 - 15 - 2013
#include<iostream>
#include<cmath>
using namespace std;

float get_float();
float calc_hypotenuse(float, float);
float calc_perimeter(float, float);
float calc_area(float, float);

int main()
{
    float side1, side2;
    float perimeter, area;
    
    //Asks the user to enter two side lengths and calls the function
    //get_float() both times.
    cout<<"Enter the length of one side: ";
    side1 = get_float();
    cout<<endl;
    cout<<"Enter the length of the adjacent side: ";
    side2 = get_float();
    cout<<endl;
    
    //Calls the calc_perimeter function and the calc_area function
    //in order to set the perimeter and area with the two lengths
    //the user input.
    perimeter = calc_perimeter(side1, side2);
    area = calc_area(side1, side2);
    
    //Outputs to the user the calculated perimeter, area, and hypotenuse.
    cout<<"A right triangle with sides "<<side1<<" and "<<side2<<" has a perimeter of "<<perimeter<<" and an area of "<<area<<". The length of the third side is "<<calc_hypotenuse(side1, side2)<<endl;

    return 0;
}

//Function that accepts a user input and ensures it is over 0
float get_float()
{
    float side;
    cin>>side;
    while(side<=0)
    {
	cout<<"Side length must be greater than 0: ";
	cin>>side;
    }
    return side;
}

//Function that accepts two floats as sides of a triangle and
//calculates the hypotenuse.
float calc_hypotenuse(float side1, float side2)
{
   float hypotenuse = sqrt(pow(side1, 2.0) + pow(side2, 2.0));
   return hypotenuse;
}

//Function that accepts two floats as sides of a triangle and
//calls the calc_hypotenuse function for the third side, then
//adds them together to calculate the perimeter
float calc_perimeter(float side1, float side2)
{
    float perimeter = side1 + side2 + calc_hypotenuse(side1, side2);
    return perimeter;
}

//Function that accepts two floats as sides of a triangle and
//calculates the area.
float calc_area(float side1, float side2)
{
    float area = (side1*side2)/2;
    return area;
}

