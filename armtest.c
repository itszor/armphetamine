#include <stdio.h>

int testfunc(void)
{
  int i, j, k=0;

  for (i=0, j=0; i<100; i++, j+=2)
    if ((i & 7) == 3)
      k += i*j;
    else
      k -= 2;
  return k;
}

int done(void) {}

int main(void)
{
  return 0;
}
