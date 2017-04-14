#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//needed for IO things. Attention that this is different from kernel mode
int lcd;
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char *)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char *)

void test ()
{
  int k, i, sum;
  char s[5] = "aeiou";

  printf("test begin!\n");

  k = write(lcd, s, sizeof(s));
  printf("written = %d\n", k);

  k = ioctl(lcd, SCULL_HELLO);
  printf ("result = %d\n", k);

  k = ioctl(lcd, SCULL_WRITE, s);
  printf ("result = %d\n", k);
  printf ("user message %s\n", s);

  char user_msg[5];
  k = ioctl(lcd, SCULL_READ, user_msg);
  printf ("result = %d\n", k);
  printf ("read results %s\n", user_msg);

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
