#include <stdio.h>

int testfunc(void)
{
  int x, y, z, i=0;

  for (z=0; z<100; z++)
  {
    for (y=0; y<100; y++)
    {
      for (x=0; x<100; x++)
      {
        i += x*y;
      }
    }
  }
  
  return i;
}

int done(void) {}

int main(void)
{
  return 0;
}
