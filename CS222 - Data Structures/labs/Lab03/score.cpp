// Author:  Melissa Danforth
// Date:    22 Jan 2007
// Purpose: Add operators to the Score class
//          The Score class allows us to implement an integer score that can
//          store a score larger than the maximum integer value allowed on
//          the system. It does so by storing the in two parts: one for the
//          low-order digits (million_counter) and one for the high-order 
//          (billion_counter) digits. This class only supports increasing 
//          scores (you can only add positive values to it), although it 
//          could be expanded to also allow subtraction.

#include <iostream>
#include <iomanip>
using namespace std;

// The maximum value for a 32-bit signed integer is 2,147,483,647
// So there's 9 digits that can be fully contained within an integer and 
// any 10-digit number over that value cannot be held in an integer. We'll
// set the threshold to 1 billion, so that the million_counter will contain 
// only 9 digits. We'll then store the 10th, 11th, 12th, etc digits in a second
// integer (billion_counter) and combine the two for displaying.
class Score
{
  private:
    // Value at which we'll shift digits from million_counter to billion_counter
    static const int THRESHOLD = 1000000000;
    // Number of digits in the million_counter at its largest value
    static const int PAD_WIDTH = 9;
    int million_counter;  // Holds the 9 least significant digits of score
    int billion_counter;  // Holds the 10th, 11th, etc digits of the score
                          // billion_counter is basically the score divided by
                          // the threshold

    // A helper function to format the output. Marked as const since the 
    // output operator passes a const Score & object
    void formatScore(ostream &) const;
    void normalizeScore();  // a helper function to set billion_counter properly

  public:
    Score();
    Score(int);

    void operator=(const Score &);  // Assigns the current score
    void operator+=(const Score &); // Adds the given score to the current score

    //**********************************************************************
    // Add definition here for following operators: 
    //    >> input the current score, call the normalizeScore helper function
    //    << output the current score, call the formatScore helper function
    //    +  add two scores and create a new score
    //**********************************************************************
};

// This uses I/O manipulation rather than printf(). It's more cumbersome,
// but allows us to operate with C++ I/O stream concept to use one function
// to output to both the screen and files
void Score::formatScore(ostream &o) const
{
  if(billion_counter) // We have 10 or more digits in the score
  {
    // setfill says to pad the beginning of million_counter with 0s
    //   so 9 becomes 000000009
    // setw says the column width is the number of digits given by PAD_WIDTH
    o << billion_counter << setfill('0') << setw(PAD_WIDTH) << million_counter;
  }
  else  // The score is 9 digits or less, just output million_counter directly
  {
    o << million_counter;
  }
}

// This checks if the million_counter is above the THRESHOLD. While it is, the 
// million_counter is lowered by THRESHOLD and billion_counter is incremented
void Score::normalizeScore()
{
  while(million_counter >= THRESHOLD)
  {
    million_counter -= THRESHOLD;
    billion_counter++;
  }
}

// Default constructor. Set variables to 0.
Score::Score()
{
  million_counter = 0;
  billion_counter = 0;
}

// Constructor that takes an int. Sets million_counter to int if it is positive 
// and adjusts billion_counter if needed. If a negative integer is passed, the 
// million_counter and billion_counter are set to 0, since we only support 
// positive million_counters right now.
Score::Score(int i)
{
  billion_counter = 0;
  if(i > 0)
  {
    million_counter = i;
    normalizeScore();
  }
  else
  {
    million_counter = 0;
  }
}

// Assigns the source Score to the current Score by copying the millions' 
// and billions' counter individually.
void Score::operator=(const Score &source)
{
  million_counter = source.million_counter;
  billion_counter = source.billion_counter;
}

// Adjusts the score by the given Score object. Adds the millions' counter
// from the source into the current millions' counter. Adds the billion's
// counter from the source into the current billions' counter. Calls the
// normalizeScore() helper function in case adding the two millions' counters
// resulted in a value >= THRESHOLD
void Score::operator+=(const Score &source)
{
  million_counter += source.million_counter;
  billion_counter += source.billion_counter;
  normalizeScore(); // convert 10 digit score into 9 digits + overflow
}


int main()
{
  Score b;
  int i, num = 0;

  cout << "Enter each integer as a value less than 2.147 billion.\n";
  cout << "If you enter integers which will add up to more than 2.147 "
       << "billion, you will see how the Score class operates.\n\n";
  cout << "Enter a number for 'a': ";
  cin >> i;

  // Set value of object 'a' using the constructor that takes an integer. This
  // converts i into a score. 
  Score a(i);

  // The num variable will show the overflows that occur if using purely just
  // an integer for these operations. Only set num if i is positive, since the
  // constructor will reject negative values.
  if(i > 0) num = i;

  cout << "\nEnter a number to adjust 'a' by. Enter a large number that is\n";
  cout << "less than 2.147 billion, but adds with 'a' to be over 2.147 billion\n";
  cout << "if you want to test integer overflow: ";
  cin >> i;

  // Add i into the score a. This will actually call the conversion constructor
  // to convert the integer i into a score, then call the += operator in the
  // Score class
  a += i;

  // Add i to num if i is positive since adj() rejects negative values
  if(i > 0) num += i;

  cout << "After adding i, 'a' is now " << a << endl;
  cout << "If we had tried to add these numbers together using just an\n";
  cout << "integer, the value would be " << num << endl;

  // For the object 'b', we will use the input operator to set the score
  // directly, rather than calling the constructor like we did for 'a'
  cout << "Enter a number for 'b': ";
  cin >> b;

  // Print out values using the output and addition operators
  cout << "'a' is " << a << endl;
  cout << "'b' is " << b << endl;
  cout << "a+b is " << (a + b) << endl;

  return 0;
}

