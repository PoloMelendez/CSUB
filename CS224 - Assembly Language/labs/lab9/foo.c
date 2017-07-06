/* foo.c
 * demonstrate gcc compiler's AT&T x86
 *
 * $ gcc -S foo.c
 * $ gcc foo.s -o foo
 * $ ./foo
 *
*/

#include <stdio.h> 

int func1(int arg)
{
  return arg * 33;
}

int main(int argc, char **argv)
{
  int temp = 77;
  temp = func1(temp);
  printf("local: %d\n", temp); 
  return temp;
}
