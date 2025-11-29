#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void troca(int *a, int *b) {
    int aux = *a;
    *a = *b;
    *b = aux;
}

int particiona(int *v, int inicio, int fim, int mode) {
    int pivo;
    if (mode == 2) {
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

void quicksort(int *v, int inicio, int fim, int mode) {
    if (inicio < fim) {
        int meio = particiona(v, inicio, fim, mode);
        quicksort(v, inicio, meio - 1, mode);
        quicksort(v, meio, fim, mode);
    }
}

void gerar_melhor_caso(int *v, int n) {
    for (int i = 0; i < n; i++) v[i] = i;
}

void gerar_pior_caso(int *v, int n) {
    for (int i = 0; i < n; i++) v[i] = n - i - 1;
}

void gerar_caso_medio(int *v, int n) {
    srand(42);
    for (int i = 0; i < n; i++) v[i] = rand() % 1000000;
}

double* repeticoes(int modo, int n, int rep, int tamanho_index, int total_tamanhos, char* caso_nome) {
    double *tempos = malloc(rep * sizeof(double));
    double soma = 0.0;
    
    printf("\n📊 [%d/%d] %s - n=%d\n", tamanho_index + 1, total_tamanhos, caso_nome, n);

    for (int k = 0; k < rep; k++) {
        int *arr = malloc(n * sizeof(int));

        if (modo == 1) gerar_melhor_caso(arr, n);
        else if (modo == 2) gerar_pior_caso(arr, n);
        else gerar_caso_medio(arr, n);

        clock_t start = clock();
        quicksort(arr, 0, n - 1, modo);
        clock_t end = clock();

        tempos[k] = (double)(end - start) / CLOCKS_PER_SEC;
        soma += tempos[k];
        
        // Mostra progresso a cada repetição
        float progresso = (k + 1) * 100.0 / rep;
        printf("   🔄 %d/%d (%.1f%%) - Tempo: %.6fs\n", k + 1, rep, progresso, tempos[k]);

        free(arr);
    }

    double media = soma / rep;
    double soma_quadrados = 0.0;
    
    for (int i = 0; i < rep; i++) {
        soma_quadrados += (tempos[i] - media) * (tempos[i] - media);
    }

    double desvio = sqrt(soma_quadrados / rep);
    double total = soma;

    static double resp[3];
    resp[0] = media;
    resp[1] = desvio;
    resp[2] = total;
    
    printf("   ✅ Finalizado - Media: %.6fs\n", media);

    free(tempos);
    return resp;
}

void gerar_csv(int *tamanhos, int qtde_tamanhos, int rep, char *nome_arquivo) {
    printf("🚀 INICIANDO TESTES DO QUICKSORT\n");
    printf("📁 Arquivo de saida: %s\n", nome_arquivo);
    printf("🔢 Tamanhos: ");
    for (int i = 0; i < qtde_tamanhos; i++) {
        printf("%d ", tamanhos[i]);
    }
    printf("\n");
    printf("🔄 Repeticoes: %d\n", rep);
    printf("📈 Progresso:\n\n");

    FILE *csv = fopen(nome_arquivo, "w");
    fprintf(csv, "TABELAS DE RESULTADOS DO QUICKSORT\n\n");

    for (int i = 0; i < qtde_tamanhos; i++) {
        int atual = tamanhos[i];

        printf("========================================\n");
        printf("🎯 VETOR TAMANHO: %d\n", atual);
        printf("========================================\n");

        // Melhor caso
        double *resp = repeticoes(1, atual, rep, i, qtde_tamanhos, "MELHOR CASO");
        fprintf(csv, "===== MELHOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo_total\n");
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, resp[0], resp[1], resp[2]);

        // Pior caso
        resp = repeticoes(2, atual, rep, i, qtde_tamanhos, "PIOR CASO");
        fprintf(csv, "===== PIOR CASO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo_total\n");
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, resp[0], resp[1], resp[2]);

        // Caso médio
        resp = repeticoes(3, atual, rep, i, qtde_tamanhos, "CASO MEDIO");
        fprintf(csv, "===== CASO MEDIO (n=%d) =====\n", atual);
        fprintf(csv, "Repeticoes,Media,Desvio,Tempo_total\n");
        fprintf(csv, "%d,%.6f,%.6f,%.6f\n\n", rep, resp[0], resp[1], resp[2]);
    }

    fclose(csv);
    printf("\n🎉 Arquivo '%s' gerado com sucesso!\n", nome_arquivo);
    printf("💾 Todos os testes concluidos!\n");
}

int main() {
    int n[] = {1000, 10000, 50000};
    int qtde = 3;
    gerar_csv(n, qtde, 20, "resultados_c.csv");
    return 0;
}