#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* for clock_gettime */

#define ARRAY_SIZE 70778880 /* 270 MB */
#define LINEAR_WRITE_TEST
#define LSH(dest,src,shift)  ((dest) = (src) << (shift))

int iArray[ARRAY_SIZE];

unsigned long long rdtsc(void)
{
  unsigned int low, High;
  asm volatile("rdtsc" : "=a" (low), "=d" (High));
  return ((unsigned long long)low | (((unsigned long long)High) << 32));
}


int main(int argc, char** argv)
{
  unsigned long long ll_Start, ll_End;
  int i;
  int result = 0;
  double d_Diff;

  if(argc < 1)
  {
    printf("Not enough command line arguments !!!\n");
    return 1;
  }

  int inval = atoi(argv[1]);
  int stride;
  LSH(stride, 1, inval);
  volatile int sink = 0;
  for (i = 0; i < ARRAY_SIZE; i += 1)
  {
    iArray[i] = 0;
  }
  ll_Start = rdtsc();
#ifdef LINEAR_WRITE_TEST
  for (i = 0; i < ARRAY_SIZE; i += stride)
  {
    iArray[i] = i;
  }
#endif
  ll_End = rdtsc();
  sink = result;
  sink += 1;
  d_Diff = ((double)(ll_End - ll_Start)/(3.6));
  printf("%lf\n", (d_Diff*stride)/ARRAY_SIZE);

  return 0;
}
