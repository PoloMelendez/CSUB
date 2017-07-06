#ifndef SCORE_H
#define SCORE_H

#include<iostream>
#include<iomanip>

using namespace std;

class Score{
    private:
	static const int THRESHOLD = 1000000000;
    	static const int PAD_WIDTH = 9;
    	int million_counter;
    	int billion_counter;

    public:
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
    	Score();
    	Score(int);
    	Score(double);
    	Score(const Score &);

    	Score& operator=(const Score &);
    	void operator+=(const Score &);
};
#endif
