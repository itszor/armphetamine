#include <stdio.h>

int testfunc(void)
{
  int i, j;

  for (i=1; i<100; i++)
  {
    j += 10000/i;
  }
  
  return j;
}

int done(void) {}

int main(void)
{
  return 0;
}
