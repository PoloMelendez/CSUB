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

  public:
    Score();
    Score(int);

    void operator=(const Score &);
    void operator+=(const Score &);

};

ostream& operator<<(ostream &out, const Score &s)
{
    s.formatScore(out);
    return out;
}

istream& operator>>(istream &in, Score &score)
{
    score.billion_counter = 0;
    int tmp = in;
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

void Score::operator=(const Score &source)
{
  million_counter = source.million_counter;
  billion_counter = source.billion_counter;
}

void Score::operator+=(const Score &source)
{
  million_counter += source.million_counter;
  billion_counter += source.billion_counter;
  normalizeScore();
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

  Score a(i);

  if(i > 0) num = i;

  cout << "\nEnter a number to adjust 'a' by. Enter a large number that is\n";
  cout << "less than 2.147 billion, but adds with 'a' to be over 2.147 billion\n";
  cout << "if you want to test integer overflow: ";
  cin >> i;

  a += i;

  if(i > 0) num += i;

  cout << "After adding i, 'a' is now " << a << endl;
  cout << "If we had tried to add these numbers together using just an\n";
  cout << "integer, the value would be " << num << endl;

  cout << "Enter a number for 'b': ";
  cin >> b;

  cout << "'a' is " << a << endl;
  cout << "'b' is " << b << endl;
  cout << "a+b is " << (a + b) << endl;

  return 0;
}

