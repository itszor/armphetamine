#include <stdio.h>

int testfunc(int q)
{
  int i, j;

  for (i=1; i<100; i++)
  {
    j += 10000/i;
  }
  
  return j * q;
}

int done(void) {}

int main(void)
{
  return 0;
}
