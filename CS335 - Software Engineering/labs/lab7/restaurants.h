/* 
 *Author: Greg Hefley
 * CMPS 335
 * Lab 07
 *
 * Restaurants.h header file to store function definitions.
 */
#ifndef RESTAURANTS_H
#define RESTAURANTS_H

#include<iostream>
#include<string>
#include<sstream>
#include<ostream>
#include<fstream>

using namespace std;

extern ofstream LOGFILE;

#define POS_IN_PROGRAM " file:"<<__FILE__<<", function:" << __FUNCTION__ <<" ,line:"<<__LINE__<<" "

class restaurants
{
    private:
      string destination;
  
    public:
      void run();
      string selection();
};
#endif

