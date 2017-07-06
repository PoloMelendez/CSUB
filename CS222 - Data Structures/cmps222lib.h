
/****************************************************************************
some general use functions,macros & defines that we can use over and over
feel free to add your own

function list:

char * GetTime()  -- returns a human readable current date and time

****************************************************************************/
#pragma once
#include <time.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


using namespace std;

#define PIP " file:"<<__FILE__<<" ,function:" << __FUNCTION__<<" ,line:"<<__LINE__<<" "
int PromptUserForInt( string text)
{
int valid_int;
cout << text << endl;

while (1)
  {
  if (cin >> valid_int)
    {
      // valid number
      return valid_int;
    }
  else
    {
      // not a valid number
      cout << "invalid value\n" <<  text << endl;
      cin.clear();
      while (cin.get() != '\n') ; // empty loop
    }
  }
}



double PromptUserForDouble( string text)
{
double valid_double;
cout << text << endl;

while (1) 
  {
  if (cin >> valid_double) 
    {
      // valid number
      return valid_double;
    } 
  else 
    {
      // not a valid number
      cout << "invalid value\n" <<  text << endl;
      cin.clear();
      while (cin.get() != '\n') ; // empty loop
    }
  }
}



// Do nothing for a period of time
// // used to slow the operation of programs
// // the parameter passed in will be approximatley in 100ths of a second
// // 25 -> quarter of a second, 50 -> half a second 
void WaitHundredth(int time)
 {
 struct timespec tim, tim2;
 tim.tv_sec = 0;
 tim.tv_nsec = time*10000000;

 nanosleep(&tim , &tim2);
 }


// // clear the screen
void ClearScreen()
 {
 system("clear");
 }


/* 
GetTime()
returns the current time as a formatted char array  
The returned string has the following format:
Www Mmm dd hh:mm:ss yyyy
Where Www is the weekday, Mmm the month in letters, dd the day of the month, hh:mm:ss the time, and yyyy the year.
The string is followed by a new-line character ('\n') and the terminating null-character.
*/
char * GetTime()
{
  time_t rawtime = time(0);
  struct tm * timeinfo;

  timeinfo = localtime ( &rawtime );
  return asctime (timeinfo); 
}


/*
bool PromptYN(string input)
display the string 'input'
give the user a prompt
if they type Y or y return true
if they type N or n return false
if they type anything else 
keep repeating till they type Y,y,N or n

usage:
if ( PromptYN("Do you like SPAM & eggs?")
  cout << "you answered Y or y" << endl;
else 
  cout << "you answered N or n" << endl;
*/


bool PromptYN(string input)
{
    while (true)
    {
        cout << input << " (Y/N)"<< endl;
        string user_input;
        cin >> user_input;
        if ( user_input == "Y" || user_input =="y")
            return true;
        if (user_input == "N" || user_input =="n")
            return false;
    }
}



/*
string Prompt4string(string input)
dispay the text 'input'

put put a prompt 
take the text (up to 499 chars) and return it as a string
*/
string Prompt4string(string input)
{
    while (true)
    {
        cout << input ;
        char user_input[500];
        cin.getline( user_input,499);
        return string(user_input);
    }
}

/*
this will try to convert a string to in integer
may throw and exception if it cant do it
*/
int StringToInt( string input)
{
    int ret_val;

    std::stringstream strStream(input);

    strStream >> ret_val;
    return ret_val;
}


bool StringToBool(string input)
{
    if(input=="true" || input =="1")
        return true;

    if(input=="false" || input =="0")
        return false;

    return false;
}


unsigned int RANDOM_SEED=0;

int CreateRandomNumber(int min, int max)
{
    if (RANDOM_SEED ==0)
    {
        RANDOM_SEED=time(NULL);
        srand(RANDOM_SEED);
    }
    return ( ( rand() % (max-min+1) ) + min);
}


void Wait(int seconds)
{
    time_t wait_till = time(NULL) + seconds;
    while ( time(NULL) < wait_till)
    {
// do nothing
    }
}
