#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//needed for IO things. Attention that this is different from kernel mode
int lcd;
#define SCULL_IOC_MAGIC 'k'
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char *)
#define SCULL_WR _IOWR(SCULL_IOC_MAGIC, 4, char *)

void test ()
{
  int k;
  char s[50] = "aeiou";
  char w[50] = "xyzwq";

  printf("test begin!\n");

  k = ioctl(lcd, SCULL_WRITE, s);
  printf ("result = %d\n", k);
  printf ("user message %s\n", s);

  k = ioctl(lcd, SCULL_WR, w);
  printf ("result = %d\n", k);
  printf ("read and write results %s\n", w);
  
}

int main (int argc, char **argv)
{
  lcd = open ("/dev/onebyte4mllseek", O_RDWR);
  if (lcd == -1)
    {
      perror ("unable to open onebyte4mllseek");
      exit (EXIT_FAILURE);
    }

  test();

  close(lcd);
  return 0;
}
