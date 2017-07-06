#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAX_SIZE 10

int main(int argc, char *argv[])
{
  u_int32_t ui;
  u_int16_t us;
  unsigned char uc;
  int si;
  short ss;
  char sc;
  int* array;
  //short and int narrowed to char
  printf("short: ");
  scanf("%hi",&ss);
  printf("int: ");
  scanf("%i",&si);
  sc = ss + si;
  printf("char = short + int\n");
  //unsigned char widened to unsigned int
  printf("sc: %hd\n", sc);
  printf("u_int_32 = char\n");
  ui = sc;
  printf("ui: %u\n", ui);

  //unsigned 16bit int to signed 32bit int
  printf("u_int_t: ");
  scanf("%u",&us);

  printf("int = u_int16_t\n");
  si = us;
  printf("int: %i\n",si);

  //ptr size
  int temp;
  printf("enter num of elements for buf: ");
  scanf("%i",&temp);
  int *buf = (int*) malloc (temp);
  int *ptr = (int*) malloc (MAX_SIZE);
 
  printf("size of ptr is 10\n");
  printf("if(sizeof(buf) <= sizeof(ptr)) then enter if statement\n");
  if(sizeof(buf) <= sizeof(ptr))
      printf("The buff is surely small enough :)\n");


 


  return 0;
}
