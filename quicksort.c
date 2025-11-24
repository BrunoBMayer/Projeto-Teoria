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
    if(mode == 2){
        pivo = v[inicio];
    } else {
        pivo = v[(inicio + fim) / 2];
    }

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
    double soma = 0;
   
    for (int k = 0; k < repeticoes; k++) {
        int *arr = malloc(n * sizeof(int));

        if (modo == 1) gerar_melhor_caso(arr, n);
        else if (modo == 2) gerar_pior_caso(arr, n);
        else  srand(time(NULL)); for (int i = 0; i < n; i++) arr[i] = rand() % 1000000;

        clock_t start = clock();
        quicksort(arr, 0, n - 1, modo);
        clock_t end = clock();

        tempos[k] = (double)(end - start) / CLOCKS_PER_SEC;
        soma += tempos[k];

        free(arr);
    }

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
void gerar_csv(int *tamanhos, int qtde_tamanhos,  int rep, const char *nome_arquivo) {
    FILE *csv = fopen(nome_arquivo, "w");
    if (!csv) {
        printf("Erro ao criar CSV!\n");
        return;
    }

    fprintf(csv, "TABELAS DE RESULTADOS DO QUICKSORT\n\n");

    for (int i = 0; i < qtde_tamanhos; i++) {
        int atual = tamanhos[i];

        // Melhor caso
        fprintf(csv, "===== MELHOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes, Media, Desvio, Tempo total\n");
        
            double *resp = repeticoes(1, atual, rep);
            fprintf(csv, "%d, %.6f, %.6f, %.6f\n", rep, resp[0], resp[1], resp[2]);
            free(resp);
        
        fprintf(csv, "\n");

        // Pior caso
        fprintf(csv, "===== PIOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo total\n");
        
            double *resp = repeticoes(2, atual, rep);
            fprintf(csv, "%d,%.6f,%.6f,%.6f\n", rep, resp[0], resp[1], resp[2]);
             printf("Pior caso: n=%d, repetição %d concluída\n", atual, rep);
            free(resp);
        
        fprintf(csv, "\n");

        // Caso médio
        fprintf(csv, "===== CASO MEDIO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo total\n");
        
            double *resp = repeticoes(3, atual, rep);
            fprintf(csv, "%d,%.6f,%.6f,%.6f\n", rep, resp[0], resp[1], resp[2]);
            free(resp);
        
        fprintf(csv, "\n");
    }

    fclose(csv);
    printf("Arquivo '%s' gerado com sucesso!\n", nome_arquivo);
}
int main() {
    int n[3] = {1000, 10000, 50000};
    gerar_csv(n, 3, 20, "resultados.csv");
    return 0;
}

