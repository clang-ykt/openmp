#define N 1024

#define _GNU_SOURCE
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// If one of the libomptarget plugins has been loaded, it means we are running
// with libomptarget. libomptarget.so is also used by LOMP, so we need to check
// for libomptarget.rtl.*.
static int isLibomptarget(struct dl_phdr_info *info, size_t size,
    void *data) {
  if (strstr(info->dlpi_name, "libomptarget.rtl") != NULL) {
    *((int *) data) = 1;
    return 1;
  }
  return 0;
}

#define TEST_NESTED     1
#define TEST_CONCURRENT 1
#define TEST_CONCURRENT_TF 1
#define TEST_PARALLEL1     1

int a[N], b[N];

int main() {
  int i;
  int error, totError = 0;


#if TEST_NESTED
  for (i=0; i<N; i++) a[i] = b[i] = i;
  #pragma omp target data map(to:b) map(from: a)
  {

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=0; j<N/4; j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=N/4; j<N/2; j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=N/2; j<3*(N/4); j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=3*(N/4); j<N; j++) a[j] = b[j]+1;
      }

      #pragma omp taskwait
  }

  error=0;
  for (i=0; i<N; i++) {
    if (a[i] != i+1) printf("%d: error %d != %d, error %d\n", i, a[i], i+1, ++error);
  }
  if (! error) {
    printf("  test with nested maps completed successfully\n");
  } else {
    printf("  test with nested maps completed with %d error(s)\n", error);
    totError++;
  }
#endif

#if TEST_CONCURRENT_TF
  for (i=0; i<N; i++) a[i] = b[i] = i;

  #pragma omp target nowait map(tofrom:a, b) 
  {
    int j;
    for(j=0; j<N/4; j++) a[j] = b[j]+1;
  }

  #pragma omp target nowait map(tofrom:a, b) 
  {
    int j;
    for(j=N/4; j<N/2; j++) a[j] = b[j]+1;
  }

  #pragma omp target nowait map(tofrom:a, b) 
  {
    int j;
    for(j=N/2; j<3*(N/4); j++) a[j] = b[j]+1;
  }

  #pragma omp target nowait map(tofrom:a, b) 
  {
    int j;
    for(j=3*(N/4); j<N; j++) a[j] = b[j]+1;
  }

  #pragma omp taskwait

  error=0;
  for (i=0; i<N; i++) {
    if (a[i] != i+1) printf("%d: error %d != %d, error %d\n", i, a[i], i+1, ++error);
  }
  if (! error) {
    printf("  test with concurrent with to/from maps completed successfully\n");
  } else {
    printf("  test with concurrent with to/from maps completed with %d error(s)\n", error);
    totError++;
  }
#endif


#if TEST_CONCURRENT
  // This test cannot run correctly with libomptarget because the library does
  // not support proper async. Fake the output in this case.
  int libomptargetInUse = 0;
  dl_iterate_phdr(isLibomptarget, &libomptargetInUse);
  if (libomptargetInUse) {
    printf("  test with concurrent maps completed successfully\n");
  } else {
    // Run actual test
    for (i=0; i<N; i++) a[i] = b[i] = i;

    #pragma omp target nowait map(to:b) map(from: a)
    {
      int j;
      for(j=0; j<N/4; j++) a[j] = b[j]+1;
    }

    #pragma omp target nowait map(to:b) map(from: a)
    {
      int j;
      for(j=N/4; j<N/2; j++) a[j] = b[j]+1;
    }

    #pragma omp target nowait map(to:b) map(from: a)
    {
      int j;
      for(j=N/2; j<3*(N/4); j++) a[j] = b[j]+1;
    }

    #pragma omp target nowait map(to:b) map(from: a)
    {
      int j;
      for(j=3*(N/4); j<N; j++) a[j] = b[j]+1;
    }

    #pragma omp taskwait

    error=0;
    for (i=0; i<N; i++) {
      if (a[i] != i+1) printf("%d: error %d != %d, error %d\n", i, a[i], i+1, ++error);
    }
    if (! error) {
      printf("  test with concurrent maps completed successfully\n");
    } else {
      printf("  test with concurrent maps completed with %d error(s)\n", error);
      totError++;
    }
  }
#endif

#if TEST_PARALLEL1
  for (i=0; i<N; i++) a[i] = b[i] = i;
  #pragma omp parallel num_threads(1)
  {
    #pragma omp target data map(to:b) map(from: a)
    {

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=0; j<N/4; j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=N/4; j<N/2; j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=N/2; j<3*(N/4); j++) a[j] = b[j]+1;
      }

      #pragma omp target nowait map(to:b) map(from: a)
      {
        int j;
        for(j=3*(N/4); j<N; j++) a[j] = b[j]+1;
      }

      #pragma omp taskwait
    }
  }

  error=0;
  for (i=0; i<N; i++) {
    if (a[i] != i+1) printf("%d: error %d != %d, error %d\n", i, a[i], i+1, ++error);
  }
  if (! error) {
    printf("  test with nested maps and Parallel 1 thread completed successfully\n");
  } else {
    printf("  test with nested maps and Parallel 1 thread completed with %d error(s)\n", error);
    totError++;
  }
#endif

  printf("completed with %d errors\n", totError);
  return totError;
}
