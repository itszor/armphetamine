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
        if (y>50 && x<25 && y<75)
          i += x+y+5;
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
