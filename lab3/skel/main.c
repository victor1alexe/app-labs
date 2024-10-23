#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_THREADS 4

// scriere in fisier
void write_file(char *filename, char *type, double time, long sum) {
    FILE *file = fopen(filename, "w+");
    fprintf(file, "Type: %s\nTime: %lf\nSum: %ld", type, time, sum);
    fclose(file);
}

int main() {
    FILE *file = fopen("input.txt", "r");
    int size, i;
    int *numbers;

    long sum1 = 0, sum2 = 0, sum3 = 0;
    double t1, t2;
    double time1, time2, time3;
    omp_set_num_threads(NO_THREADS);

    // de paralelizat toata bucata de mai jos

    // fiti atenti la partea de citire din fisier
    fscanf(file, "%d", &size);
    numbers = malloc(size * sizeof(int));

#pragma omp parallel for shared(size, file)
    for (int i = 0; i < size; i++) {
        fscanf(file, "%d", &numbers[i]);
    }
    fclose(file);

    // de paralelizat in 3 moduri - atomic, critical si reduction, de masurat timpii de executie
    for (i = 0; i < size; i++) {
        sum1 += numbers[i];
    }

    printf("sum seq=%ld\n", sum1);

    sum1 = 0;
    sum2 = 0;
    sum3 = 0;

// atomic
    t1 = omp_get_wtime();
#pragma omp parallel for private(i) shared(sum1, size)
    for (i = 0; i < size; i++) {
#pragma omp atomic
        sum1 += numbers[i];
    }
    t2 = omp_get_wtime();
    printf("sum atomic=%ld\n", sum1);
    time1 = t2 - t1;

// critical
    t1 = omp_get_wtime();
#pragma omp parallel for private(i) shared(sum1, size)
    for (i = 0; i < size; i++) {
#pragma omp critical
        sum2 += numbers[i];
    }
    t2 = omp_get_wtime();
    printf("sum critical=%ld\n", sum2);
    time2 = t2 - t1;

// reduction
    t1 = omp_get_wtime();
#pragma omp parallel for private(i) shared(size) reduction(+:sum3)
    for (i = 0; i < size; i++) {
#pragma omp critical
        sum3 += numbers[i];
    }
    t2 = omp_get_wtime();
    printf("sum critical=%ld\n", sum3);
    time3 = t2 - t1;


    // TODO: de scris timpii de executie in 3 fisiere folosind sections (fiecare scriere intr-un section)
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            FILE *out1 = fopen("out1", "w");
            if (!out1) {
                perror("out1");
            }

            fprintf(out1, "time=%f", time1);

            fclose(out1);
        }

        #pragma omp section
        {
            FILE *out2 = fopen("out2", "w");
            if (!out2) {
                perror("out2");
            }

            fprintf(out2, "time=%f", time2);

            fclose(out2);
        }

        #pragma omp section
        {
            FILE *out3 = fopen("out3", "w");
            if (!out3) {
                perror("out3");
            }

            fprintf(out3, "time=%f", time3);

            fclose(out3);
        }
    }
}

    return 0;
}