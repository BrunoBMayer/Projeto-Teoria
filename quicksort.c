#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void troca(int *a, int *b) {
    int aux = *a;
    *a = *b;
    *b = aux;
}

int particiona(int *v, int inicio, int fim, int mode){
    int pivo;
    if (mode == 2)      // pior caso
        pivo = v[inicio];
    else               // médio e melhor
        pivo = v[(inicio + fim) / 2];

    int i = inicio;
    int j = fim;

    while (i <= j) {
        while (v[i] < pivo) i++;
        while (v[j] > pivo) j--;
        if (i <= j) {
            troca(&v[i], &v[j]);
            i++;
            j--;
        }
    }
    return i;
}

void quicksort(int *v, int inicio, int fim, int mode){
    if (inicio < fim) {
        int meio = particiona(v, inicio, fim, mode);
        quicksort(v, inicio, meio - 1, mode);
        quicksort(v, meio, fim, mode);
    }
}

void gerar_melhor_caso(int *arr, int n) {
    for (int i = 0; i < n; i++)
        arr[i] = i;
}

void gerar_pior_caso(int *arr, int n) {
    for (int i = 0; i < n; i++)
        arr[i] = i;
}

double *repeticoes(int modo, int n, int repeticoes) {
    double *tempos = malloc(repeticoes * sizeof(double));
    double soma = 0.0;

    int *arr = malloc(n * sizeof(int));

    for (int k = 0; k < repeticoes; k++) {

        if (modo == 1)
            gerar_melhor_caso(arr, n);
        else if (modo == 2)
            gerar_pior_caso(arr, n);
        else {
            for (int i = 0; i < n; i++)
                arr[i] = rand() % 1000000;
        }

        clock_t start = clock();
        quicksort(arr, 0, n - 1, modo);
        clock_t end = clock();

        tempos[k] = (double)(end - start) / CLOCKS_PER_SEC;
        soma += tempos[k];
    }

    free(arr);

    double media = soma / repeticoes;

    double soma_quadrados = 0;
    for (int i = 0; i < repeticoes; i++)
        soma_quadrados += (tempos[i] - media) * (tempos[i] - media);

    double desvio = sqrt(soma_quadrados / repeticoes);

    double *resp = malloc(3 * sizeof(double));
    resp[0] = media;
    resp[1] = desvio;
    resp[2] = soma;

    free(tempos);
    return resp;
}

void gerar_csv(int *tamanhos, int qtde_tamanhos, int rep, const char *nome_arquivo) {
    FILE *csv = fopen(nome_arquivo, "w");
    if (!csv) {
        printf("Erro ao criar CSV!\n");
        return;
    }

    fprintf(csv, "TABELAS DE RESULTADOS DO QUICKSORT\n\n");

    for (int i = 0; i < qtde_tamanhos; i++) {
        int atual = tamanhos[i];

        fprintf(csv, "===== MELHOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo total\n");
        double *r1 = repeticoes(1, atual, rep);
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, r1[0], r1[1], r1[2]);
        free(r1);

        fprintf(csv, "===== PIOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo total\n");
        double *r2 = repeticoes(2, atual, rep);
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, r2[0], r2[1], r2[2]);
        free(r2);

        fprintf(csv, "===== CASO MEDIO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo total\n");
        double *r3 = repeticoes(3, atual, rep);
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, r3[0], r3[1], r3[2]);
        free(r3);
    }

    fclose(csv);
    printf("Arquivo '%s' gerado com sucesso!\n", nome_arquivo);
}

int main() {
    srand(time(NULL));  // importante!
    int n[3] = {1000, 10000, 50000};
    gerar_csv(n, 3, 20, "resultados_c.csv");
    return 0;
}
