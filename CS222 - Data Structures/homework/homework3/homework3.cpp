#include <iostream>
#include <iomanip>
using namespace std;

class Score
{
  private:
    static const int THRESHOLD = 1000000000;
    static const int PAD_WIDTH = 9;
    int million_counter;
    int billion_counter;

    void formatScore(ostream &) const;
    void normalizeScore();

    friend istream& operator>>(istream &, Score &);
    friend ostream& operator<<(ostream &, const Score &);
    friend Score operator+(const Score &, const Score &);
    friend Score operator-(const Score &, const Score &);

    friend bool operator<(const Score &, const Score &);
    friend bool operator<=(const Score &, const Score &);
    friend bool operator>(const Score &, const Score &);
    friend bool operator>=(const Score &, const Score &);
    friend bool operator==(const Score &, const Score &);
    friend bool operator!=(const Score &, const Score &);

  public:
    Score();
    Score(int);
    Score(double);
    Score(const Score &);

    Score& operator=(const Score &);
    void operator+=(const Score &);

};

bool operator<(const Score &score1, const Score &score2)
{
    if(score1.billion_counter > score2.billion_counter)
	return false;
    else if(score1.billion_counter == score2.billion_counter)
    {
	if(score1.million_counter > score2.million_counter)
	    return false;
	else if(score1.million_counter == score2.million_counter)
	    return false;
	else
	    return true;
    }
    else
	return true;
}
bool operator<=(const Score &score1, const Score &score2)
{
    if(score1.billion_counter > score2.billion_counter)
	return false;
    else if(score1.billion_counter == score2.billion_counter)
    {
	if(score1.million_counter > score2.million_counter)
	    return false;
	else
	    return true;
    }
    else
	return true;
}
bool operator>=(const Score &score1, const Score &score2)
{
    if(score1.billion_counter < score2.billion_counter)
	return false;
    else if(score1.billion_counter == score2.billion_counter)
    {
	if(score1.million_counter < score2.million_counter)
	    return false;
	else
	    return true;
    }
    else
	return true;
}
bool operator>(const Score &score1, const Score &score2)
{
    if(score1.billion_counter < score2.billion_counter)
	return false;
    else if(score1.billion_counter == score2.billion_counter)
    {
	if(score1.million_counter < score2.million_counter)
	    return false;
	else if(score1.million_counter == score2.million_counter)
	    return false;
	else
	    return true;
    }
    else
	return true;
}
bool operator==(const Score &score1, const Score &score2)
{
    if(score1.billion_counter == score2.billion_counter && score1.million_counter == score2.million_counter)
	return true;
    else
	return false;
}

bool operator!=(const Score &score1, const Score &score2)
{
    if(score1.billion_counter == score2.billion_counter && score1.million_counter == score2.million_counter)
	return false;
    else
	return true;
}

Score::Score(double number)
{
    million_counter = 0;
    billion_counter = 0;
    if(number < 0)
    {
	million_counter = 0;
	billion_counter = 0;
    }
    else
    {
	while(number >= THRESHOLD)
	{
	    billion_counter++;
	    number = number - THRESHOLD;
	}
	million_counter = (int) number;
    }
}

Score::Score(const Score &score)
{
    million_counter = score.million_counter;
    billion_counter = score.billion_counter;
}

ostream& operator<<(ostream &out, const Score &s)
{
    s.formatScore(out);
    return out;
}

istream& operator>>(istream &in, Score &score)
{
    score.billion_counter = 0;
    int tmp;
    in >> tmp;
    if( tmp > 0 )
    {
	score.million_counter = tmp;
	score.normalizeScore();
    }
    else
    {
	score.million_counter = 0;
    }
}

Score operator+(const Score &score1, const Score &score2)
{
    Score tmp;
    tmp.million_counter = score1.million_counter + score2.million_counter;
    tmp.billion_counter = score1.billion_counter + score2.billion_counter;
    tmp.normalizeScore();
    return tmp;
}

Score operator-(const Score &score1, const Score &score2)
{
    Score tmp(score1);
    if(score2 > score1)
    {
	tmp.million_counter = 0;
	tmp.billion_counter = 0;
    }
    else
    {
	if(score2.million_counter > score1.million_counter)
	{
	    tmp.billion_counter--;
	    tmp.million_counter+=1000000000;
	}
	tmp.million_counter = score1.million_counter - score2.million_counter;
	tmp.billion_counter = score1.billion_counter - score2.billion_counter;
    }
    return tmp;
}

void Score::formatScore(ostream &o) const
{
  if(billion_counter)
  {
    o << billion_counter << setfill('0') << setw(PAD_WIDTH) << million_counter;
  }
  else
  {
    o << million_counter;
  }
}

void Score::normalizeScore()
{
  while(million_counter >= THRESHOLD)
  {
    million_counter -= THRESHOLD;
    billion_counter++;
  }
}

Score::Score()
{
  million_counter = 0;
  billion_counter = 0;
}

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

Score& Score::operator=(const Score &source)
{
  million_counter = source.million_counter;
  billion_counter = source.billion_counter;
  return *this;
}

void Score::operator+=(const Score &source)
{
  million_counter += source.million_counter;
  billion_counter += source.billion_counter;
  normalizeScore();
}


int main(){
    Score b;
    double i;

    cout << "Enter a number for 'a': ";
    cin >> i;

    // Set value of object 'a' using the constructor that takes a double. This
    // converts i into a score. 
    Score a(i);
    cout << "'a' is " << a << endl;

    cout << "\nEnter a number to adjust 'a' by: ";
    cin >> i;

    // Adjust the value of a. This will also use automatic type conversion to
    // convert i into a Score, then call the update operator to add it into a.
    a += i;

    cout << "After adjust, 'a' is now " << a << endl;

    // For the object 'b', we will use the input operator to set the score
    // directly, rather than calling the constructor like we did for 'a'
    cout << "\nEnter a number for 'b': ";
    cin >> b;

    Score clone(b);  // Call the copy constructor

    cout << "'b' is " << b << endl;
    cout << "The clone of 'b' is " << clone << endl;

    clone = a + b;  // Assign the result of adding two scores into clone

    cout << "\nAfter 'clone' = 'a' + 'b', 'clone' is now " << clone << endl;

    cout << "\nTesting subtraction...\n        a is " << a << endl;
    cout << "        b is " << b << endl << "  (a - b) is " << (a - b) << endl;
    cout << "After subtraction, a is " << a << endl;

    // Print out values using the output and addition operators
    cout << "\nTesting comparison operators.\n";
    cout << "'a' is " << a << endl;
    cout << "'b' is " << b << endl;
    cout << endl;

    if(a < b)
      cout << "'a' is less than 'b'.\n";
    else
      cout << "'a' is NOT less than 'b'.\n";

    if(a <= b)
      cout << "'a' is less or equal to 'b'.\n";
    else
      cout << "'a' is NOT less or equal to 'b'.\n";

    if(a > b)
      cout << "'a' is greater than 'b'.\n";
    else
      cout << "'a' is NOT greater than 'b'.\n";

    if(a >= b)
      cout << "'a' is greater than or equal to 'b'.\n";
    else
      cout << "'a' is NOT greater than or equal to 'b'.\n";

    if(a == b)
      cout << "'a' is equal to 'b' (equality operator).\n";
    else
      cout << "'a' is NOT equal to 'b' (equality operator).\n";

    if(a != b)
      cout << "'a' is NOT equal to 'b' (inequality operator).\n";
    else
      cout << "'a' is equal to 'b' (inequality operator).\n";

return 0;
}

  
