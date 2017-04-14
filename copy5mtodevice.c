#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  FILE* fp;
  char data[5*1024*1024];
  size_t count = 0;

  memset(data, '0', sizeof(data));
  data[0] = 'S';
  for (long int i = 1; i < sizeof(data)/sizeof(char)-1; i++)
    {
      if (i%2 == 0) data[i] = 'a';
      else if (i%79 == 0) data[i] = '\n';
      else data[i] = 'b';
    }
  data[-1] = 'E';
  fp = fopen("/dev/onebyte4m", "w");
  count = fwrite(data,  sizeof(char), sizeof(data)/sizeof(char), fp);
  fclose(fp);
  printf("%lu\n", count);

  return 1; // Copy 5m to 4m cause overflow. Return error. 
}
