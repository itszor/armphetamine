#include <stdio.h>

int testfunc(void)
{
  int i, j, k=0;

  for (i=0; i<100; i++)
    for (j=0; j<100; j++)
      if ((j^17)==(i^3))
        k += i*j;

  return k;
}

int done(void) {}

int main(void)
{
  return 0;
}