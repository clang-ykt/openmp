#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int a = 1;
  int b = 2;

  #pragma omp parallel
  #pragma omp single
  {
    #pragma omp target enter data nowait map(to: a) depend(out: a)
    #pragma omp task depend(inout: a) shared(b)
    { b++; /* printf("hi alex\n");*/ }
    #pragma omp target exit data nowait map(from: a) depend(in: a)
  }
  if (b==3) printf("success\n");
  else printf("horrible failure\n");
  return EXIT_SUCCESS;
}
