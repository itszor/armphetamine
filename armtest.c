#include <stdio.h>

int testfunc(int q)
{
  int i, j;

  for (i=1; i<100; i++)
  {
    j += (i+j*3+17) ^ 0x35;
  }
  
  return j * q;
}

int done(void) {}

int main(void)
{
  return 0;
}
