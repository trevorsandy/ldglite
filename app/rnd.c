// generate random 20% stipple patterns for cheesy translucency.

#include <stdio.h>

char buf[10240];

/***************************************************************/
main(int *argc, char **argv)
{

  int i,j,k,n;
  int bit;
  int byte;

  n = rand(75);
  bit = 1;
  byte = 0;

  for (i = 0; i < 10; i++)
  {
    printf( "GLubyte pattern%d[] = {\n  ", i);

    for (j = 0; j < 32; j++)
    {
      for (k = 0; k < 32; k++)
      {
	n = rand();
	// get 20 % of bits on
	if ((n % 5) == 0)
	  byte |= bit;
      
	bit *= 2;
	if (bit == 256)
	{
	  printf("0x%02X, ",byte);
	  bit = 1;
	  byte = 0;
	}
      }
      if ((j % 2) == 1)
	printf( "\n  " );
    }

    printf( "};\n\n" );
  }
}


