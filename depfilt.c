#include <stdio.h>

int main(int argc, char* argv[])
{
  int c;

  if (argc != 2) return 1;

  printf("%s/", argv[1]);

  while ((c = getchar()) != EOF)
  {
    putchar(c);
  }

  return 0;
}
