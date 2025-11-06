// kruskal_quicksort_benchmark_csv.c
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

/* ========== Cronômetro (Windows e POSIX) ========== */
static double now_ms(void) {
#ifdef _WIN32
    static LARGE_INTEGER freq; static int init = 0;
    if (!init) { QueryPerformanceFrequency(&freq); init = 1; }
    LARGE_INTEGER t; QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    #if defined(CLOCK_MONOTONIC)
        clock_gettime(CLOCK_MONOTONIC, &ts);
    #else
        clock_gettime(CLOCK_REALTIME, &ts);
    #endif
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1.0e6;
#endif
}

/* ========== Tipos básicos ========== */
typedef struct { int u, v, w; } Edge;

typedef struct {
    int n;
    int *parent;
    int *size;
} DSU;

/* ========== DSU (Union-Find) ========== */
static DSU* dsu_new(int n) {
    DSU *d = (DSU*)malloc(sizeof(DSU));
    d->n = n;
    d->parent = (int*)malloc(n * sizeof(int));
    d->size   = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { d->parent[i] = i; d->size[i] = 1; }
    return d;
}

static int dsu_find(DSU *d, int a) {
    if (d->parent[a] != a) d->parent[a] = dsu_find(d, d->parent[a]);
    return d->parent[a];
}

static int dsu_union(DSU *d, int a, int b) {
    int ra = dsu_find(d, a), rb = dsu_find(d, b);
    if (ra == rb) return 0;
    if (d->size[ra] < d->size[rb]) { int t = ra; ra = rb; rb = t; }
    d->parent[rb] = ra;
    d->size[ra]  += d->size[rb];
    return 1;
}

static void dsu_free(DSU *d) {
    free(d->parent); free(d->size); free(d);
}

/* ========== Quicksort por peso (didático) ========== */
static inline void swap_edge(Edge *a, Edge *b) { Edge t = *a; *a = *b; *b = t; }

static int partition_edges(Edge *A, int lo, int hi) {
    int p = lo + rand() % (hi - lo + 1);            // pivô aleatório
    swap_edge(&A[p], &A[hi]);
    int x = A[hi].w;
    int i = lo;
    for (int j = lo; j < hi; j++) {
        if (A[j].w <= x) { swap_edge(&A[i], &A[j]); i++; }
    }
    swap_edge(&A[i], &A[hi]);
    return i;
}

static void quicksort_edges(Edge *A, int lo, int hi) {
    if (lo >= hi) return;
    int p = partition_edges(A, lo, hi);
    quicksort_edges(A, lo, p - 1);
    quicksort_edges(A, p + 1, hi);
}

/* ========== Kruskal ========== */
typedef struct {
    int ok;
    long long custo;
    double tempo_sort_ms;
    double tempo_total_ms;
} KrRes;

static KrRes kruskal(int n, Edge *E, int m) {
    double t0 = now_ms();
    quicksort_edges(E, 0, m - 1);
    double t1 = now_ms();

    DSU *dsu = dsu_new(n);
    long long total = 0;
    int usados = 0;

    for (int i = 0; i < m && usados < n - 1; i++) {
        if (dsu_union(dsu, E[i].u, E[i].v)) {
            total += E[i].w;
            usados++;
        }
    }
    double t2 = now_ms();
    dsu_free(dsu);

    KrRes r;
    r.ok = (usados == n - 1);
    r.custo = r.ok ? total : -1;
    r.tempo_sort_ms  = (t1 - t0);
    r.tempo_total_ms = (t2 - t0);
    return r;
}

/* ========== Geração de grafo conexo ========== */
// chave única para par (u,v) com u < v
static inline long long par_key(int u, int v, int n) {
    if (u > v) { int t = u; u = v; v = t; }
    return (long long)u * n + v;
}

static int tem_par(long long *keys, int len, long long k) {
    for (int i = 0; i < len; i++) if (keys[i] == k) return 1;
    return 0;
}

static void embaralhar(int *a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

static Edge* grafo_conexo_aleatorio(int n, int m, int *m_out, unsigned seed) {
    if (m < n - 1) return NULL;
    long long max_edges = (long long)n * (n - 1) / 2;
    if (m > max_edges) return NULL;

    srand(seed);

    Edge *E = (Edge*)malloc(m * sizeof(Edge));
    long long *keys = (long long*)malloc(m * sizeof(long long));
    int usado = 0;

    // 1) Árvore base via permutação (garante conexão)
    int *perm = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) perm[i] = i;
    embaralhar(perm, n);

    for (int i = 1; i < n; i++) {
        int u = perm[i - 1], v = perm[i];
        if (u > v) { int t = u; u = v; v = t; }
        long long k = par_key(u, v, n);
        E[usado] = (Edge){u, v, 1 + rand() % 100};
        keys[usado++] = k;
    }
    free(perm);

    // 2) Arestas extras distintas
    while (usado < m) {
        int u = rand() % n, v = rand() % n;
        if (u == v) continue;
        if (u > v) { int t = u; u = v; v = t; }
        long long k = par_key(u, v, n);
        if (tem_par(keys, usado, k)) continue;
        E[usado] = (Edge){u, v, 1 + rand() % 100};
        keys[usado++] = k;
    }

    free(keys);
    *m_out = usado;
    return E;
}

/* ========== Benchmark com CSV + Resumo ========== */
typedef struct {
    const char *nome;
    int n;
    int m;
} Caso;

static void benchmark_para_csv(
    const char *saida_csv,
    const Caso *casos, int num_casos,
    int repeticoes,
    unsigned seed_base
) {
    FILE *fp = fopen(saida_csv, "w");
    if (!fp) { fprintf(stderr, "Nao foi possivel abrir %s para escrita.\n", saida_csv); return; }

    // Cabeçalho do CSV
    fprintf(fp, "caso,V,E,seed,execucao,custo,tempo_sort_ms,tempo_total_ms,ok\n");

    // agregadores para resumo
    double *sum_sort = (double*)calloc(num_casos, sizeof(double));
    double *sum_total= (double*)calloc(num_casos, sizeof(double));
    int    *ok_all   = (int*)malloc(num_casos * sizeof(int));
    long long *custo_exemplo = (long long*)malloc(num_casos * sizeof(long long));

    for (int i = 0; i < num_casos; i++) {
        ok_all[i] = 1;
        custo_exemplo[i] = -1;
    }

    for (int ci = 0; ci < num_casos; ci++) {
        const char *nome = casos[ci].nome;
        int n = casos[ci].n, m = casos[ci].m;

        for (int k = 0; k < repeticoes; k++) {
            unsigned seed = seed_base + (unsigned)k;
            int m_real = 0;
            Edge *E = grafo_conexo_aleatorio(n, m, &m_real, seed);
            if (!E) { fprintf(stderr, "Falha ao gerar grafo (%s)\n", nome); continue; }

            KrRes r = kruskal(n, E, m_real);

            // CSV (uma linha por execução)
            fprintf(fp, "%s,%d,%d,%u,%d,%lld,%.6f,%.6f,%d\n",
                    nome, n, m, seed, k+1, r.custo, r.tempo_sort_ms, r.tempo_total_ms, r.ok);

            // agregadores
            if (k == 0) custo_exemplo[ci] = r.custo;
            sum_sort[ci]  += r.tempo_sort_ms;
            sum_total[ci] += r.tempo_total_ms;
            if (!r.ok) ok_all[ci] = 0;

            free(E);
        }
    }

    fclose(fp);

    // ===== Imprime resumo por caso (formato compacto) =====
    for (int ci = 0; ci < num_casos; ci++) {
        const char *nome = casos[ci].nome;
        int n = casos[ci].n, m = casos[ci].m;

        double avg_sort  = sum_sort[ci]  / (double)repeticoes;
        double avg_total = sum_total[ci] / (double)repeticoes;

        printf("\n== Caso %s ==\n", nome);
        printf("V=%d, E=%d\n", n, m);
        printf("MST ok? %s\n", ok_all[ci] ? "True" : "False");
        printf("Custo MST: %lld\n", custo_exemplo[ci]);
        printf("Tempo sort:  %.2f ms\n", avg_sort);
        printf("Tempo total: %.2f ms\n", avg_total);
    }

    printf("\nCSV salvo em: %s\n", saida_csv);

    free(sum_sort);
    free(sum_total);
    free(ok_all);
    free(custo_exemplo);
}

int main(void) {
    const Caso casos[] = {
        {"pequena", 50,   200},
        {"média",   300,  5000},
        {"grande",  1000, 20000}
    };
    const int num_casos = (int)(sizeof(casos) / sizeof(casos[0]));

    // Ajuste aqui se quiser:
    const char *arquivo_csv = "resultados_kruskal_quicksort.csv";
    const int repeticoes = 5;
    const unsigned seed_base = 123u;

    benchmark_para_csv(arquivo_csv, casos, num_casos, repeticoes, seed_base);
    return 0;
}
