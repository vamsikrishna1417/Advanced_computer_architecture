#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* for clock_gettime */

#define POINTER_SIZE       (sizeof(void*))
#define CACHE_LINE_SIZE    64
#define POINTERS_PER_LINE  (CACHE_LINE_SIZE / POINTER_SIZE)

unsigned long long rdtsc(void)
{
  unsigned int low, High;
  asm volatile("rdtsc" : "=a" (low), "=d" (High));
  return ((unsigned long long)low | (((unsigned long long)High) << 32));
}

typedef struct __CacheLine {
  int pCacheLine[POINTERS_PER_LINE] ;
} sCacheLine;

int main(int argc, char** argv)
{
  unsigned long long ll_Start = 0, ll_End = 0;
  double d_Diff = 0.0;
  int i = 0;
  int index = 0;
  int i_InputVal = 0;
  int ll_Array_elements ;
  sCacheLine* pCacheArray = NULL ;

  if(argc < 1)
  {
    printf("Not enough command line arguments !!!\n");
    return 1;
  }

  i_InputVal = atoi(argv[1]);
  ll_Array_elements = (1 << i_InputVal) ;
  pCacheArray = (sCacheLine*) malloc(ll_Array_elements*sizeof(sCacheLine)) ;

  /* Initializing the entire pCacheArray to 0 */
  for (i = 0 ; i < ll_Array_elements ; i++)
  {
    pCacheArray[i].pCacheLine[0] = 0 ;
  }

  ll_Start = rdtsc();
  for (i = 0; i < ll_Array_elements; ++i)
  {
    pCacheArray[index].pCacheLine[0] = i;
    index = (index + i) & (ll_Array_elements-1);
  }
  ll_End = rdtsc();
  d_Diff = ((double)(ll_End - ll_Start)/(3.6));
  printf("%lf\n", (d_Diff)/ll_Array_elements);

  return 0;
}
