#include<iostream>
#include<iomanip>
#include "score.h"

using namespace std;

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
	while(number > THRESHOLD)
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


