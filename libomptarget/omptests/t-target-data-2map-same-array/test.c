#include <stdio.h>

int main()
{
  int a[100];
  int *p = &a[0];
  int i;

  for(i=0; i<100; i++) a[i] = i;

  #pragma omp target data map(tofrom: a, p[0:100])
  {
    #pragma omp target map(tofrom: a, p[0:100])
    {
      int *q = p;
      for(i=0; i<100; i++) {
        a[i]++;
        *q = (*q) + 1;
        q++;
      }
    }
  }
  int error = 0;
  for(i=0; i<100; i++) if (a[i] != i+2) printf("%d, got %d, wanted %d, error %d\n", i, a[i], i+2, ++error);
  printf("finished with %d errors\n", error);

  return 1;
}
