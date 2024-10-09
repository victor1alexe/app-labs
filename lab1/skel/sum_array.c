#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 100

int main() {
    int a[SIZE], i;
    long sum = 0;
    
    double t1 = omp_get_wtime();

#pragma omp parallel for private(i) shared(a)
    for (i = 0; i < SIZE; i++) {
        a[i] = i;
    }

#pragma omp parallel for reduction(+:sum) private(i)
    for (i = 0; i < SIZE; i++) {
// #pragma omp critical
        sum += a[i];
    }

    double t2 = omp_get_wtime();

    // printf("Total sum = %ld\n", sum);
    printf("Total execution time = %lf\n", (t2 - t1));

    return 0;
}
