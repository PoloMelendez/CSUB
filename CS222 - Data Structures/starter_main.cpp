/*
*  Your Full Name
*  CMPS 222
*  Spring 2011
*/
#include "/usr/stu/YOUR_USERNAME/assignments/cmps222/cmps222lib.h"   
ofstream LOGFILE;


// your functions or file includes


int main()
{
LOGFILE.open("errorlog.log",ios::trunc);
if (LOGFILE.is_open())
  LOGFILE <<  __FILE__ << " : " << __FUNCTION__ << " : line " << __LINE__ << " : " << "start of logging" << endl;
else 
  {
  cout << "Unable to open file for logging.";
  return 1;
  }
	
// now all your regular stuff can go here



// close the logfile and exit
LOGFILE.close();
return 0;
}
