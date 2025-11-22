#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct {
    int N;
    const char *Caso;
    double mean_time;
    double std_dev_time;
} Result;

typedef struct {
    int size;
    const char *case_type;
} TestCase;

double current_time_seconds(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

int partition(int *arr, int low, int high) {
    int range = high - low + 1;
    int rand_index = low + rand() % range;

    int tmp = arr[high];
    arr[high] = arr[rand_index];
    arr[rand_index] = tmp;

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            int tmp2 = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp2;
        }
    }

    int tmp3 = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = tmp3;

    return i + 1;
}

void quick_sort(int *arr, int low, int high) {
    if (low >= high) {
        return;
    }

    int n = high - low + 1;

    int (*stack)[2] = malloc(n * sizeof *stack);
    if (!stack) {
        fprintf(stderr, "Erro de alocacao de memoria na pilha do quick_sort.\n");
        exit(EXIT_FAILURE);
    }

    int top = 0;            

    stack[top][0] = low;    
    stack[top][1] = high;   
    top++;

    while (top > 0) {
        top--;
        int l = stack[top][0];
        int h = stack[top][1];

        if (l < h) {
            int p = partition(arr, l, h);

            if (p + 1 < h) {
                stack[top][0] = p + 1;
                stack[top][1] = h;
                top++;
            }
            if (l < p - 1) {
                stack[top][0] = l;
                stack[top][1] = p - 1;
                top++;
            }
        }
    }

    free(stack);
}

int *generate_random_array(int size) {
    int *arr = (int *)malloc(size * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Erro de alocacao de memoria em generate_random_array.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        arr[i] = rand() % (size * 10 + 1);
    }

    return arr;
}

int *generate_best_case_array(int size) {
    int *arr = (int *)malloc(size * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Erro de alocacao de memoria em generate_best_case_array.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return arr;
}

int *generate_worst_case_array(int size) {
    int *arr = (int *)malloc(size * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Erro de alocacao de memoria em generate_worst_case_array.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        arr[i] = size - i;
    }

    return arr;
}

void run_tests(void) {
    TestCase tests[] = {
        {1000, "melhor"},
        {2000, "medio"},
        {5000, "pior"}
    };
    int num_tests = (int)(sizeof(tests) / sizeof(tests[0]));
    int num_runs = 30;

    printf("--- Resultados QuickSort ---\n");
    printf("Tamanho (N) | Caso       | Media (s) | Desvio Padrao (s)\n");
    printf("-------------------------------------------------------\n");

    Result *results = (Result *)malloc(num_tests * sizeof(Result));
    if (!results) {
        fprintf(stderr, "Erro de alocacao de memoria em results.\n");
        exit(EXIT_FAILURE);
    }

    for (int t = 0; t < num_tests; t++) {
        int size = tests[t].size;
        const char *case_type = tests[t].case_type;

        double *execution_times = (double *)malloc(num_runs * sizeof(double));
        if (!execution_times) {
            fprintf(stderr, "Erro de alocacao de memoria em execution_times.\n");
            exit(EXIT_FAILURE);
        }

        for (int run = 0; run < num_runs; run++) {
            int *arr;

            if (case_type[0] == 'm' && case_type[1] == 'e') {          
                arr = generate_best_case_array(size);
            } else if (case_type[0] == 'p') {                          
                arr = generate_worst_case_array(size);
            } else {                                                 
                arr = generate_random_array(size);
            }

            int *arr_copy = (int *)malloc(size * sizeof(int));
            if (!arr_copy) {
                fprintf(stderr, "Erro de alocacao de memoria em arr_copy.\n");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < size; i++) {
                arr_copy[i] = arr[i];
            }

            double start_time = current_time_seconds();
            quick_sort(arr_copy, 0, size - 1);
            double end_time = current_time_seconds();

            execution_times[run] = end_time - start_time;

            free(arr);
            free(arr_copy);
        }

        double avg_time = 0.0;
        for (int i = 0; i < num_runs; i++) {
            avg_time += execution_times[i];
        }
        avg_time /= num_runs;

        double variance = 0.0;
        if (num_runs > 1) {
            for (int i = 0; i < num_runs; i++) {
                double diff = execution_times[i] - avg_time;
                variance += diff * diff;
            }
            variance /= (num_runs - 1);
        }
        double std_dev = (num_runs > 1) ? sqrt(variance) : 0.0;

        printf("%-11d | %-10s | %.6f | %.6f\n", size, case_type, avg_time, std_dev);

        results[t].N = size;
        results[t].Caso = case_type;
        results[t].mean_time = avg_time;
        results[t].std_dev_time = std_dev;

        free(execution_times);
    }

    FILE *csvfile = fopen("quicksort_results_python.csv", "w");
    if (!csvfile) {
        fprintf(stderr, "Nao foi possivel abrir o arquivo CSV para escrita.\n");
        free(results);
        exit(EXIT_FAILURE);
    }

    fprintf(csvfile, "N,Caso,mean_time,std_dev_time\n");
    for (int i = 0; i < num_tests; i++) {
        fprintf(csvfile, "%d,%s,%.10f,%.10f\n",
                results[i].N,
                results[i].Caso,
                results[i].mean_time,
                results[i].std_dev_time);
    }

    fclose(csvfile);
    free(results);

    printf("\nArquivo gerado: quicksort_results_python.csv\n");
}

int main(void) {
    srand((unsigned int)time(NULL));

    run_tests();

    return 0;
}
