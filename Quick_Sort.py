# kruskal_quicksort_csv.py
import random
import time
import csv
import statistics as stats
from pathlib import Path

# =================== DSU (Union-Find) ===================
class DSU:
    def __init__(self, n: int):
        self.parent = list(range(n))
        self.size = [1] * n

    def find(self, a: int) -> int:
        if self.parent[a] != a:
            self.parent[a] = self.find(self.parent[a])
        return self.parent[a]

    def union(self, a: int, b: int) -> bool:
        ra, rb = self.find(a), self.find(b)
        if ra == rb:
            return False
        if self.size[ra] < self.size[rb]:
            ra, rb = rb, ra
        self.parent[rb] = ra
        self.size[ra] += self.size[rb]
        return True

# =================== Quicksort (por peso) ===================
def _partition(edges, lo, hi):
    p = random.randint(lo, hi)                # pivô aleatório
    edges[p], edges[hi] = edges[hi], edges[p]
    x = edges[hi][0]                          # peso
    i = lo
    for j in range(lo, hi):
        if edges[j][0] <= x:
            edges[i], edges[j] = edges[j], edges[i]
            i += 1
    edges[i], edges[hi] = edges[hi], edges[i]
    return i

def quicksort(edges, lo=0, hi=None):
    if hi is None:
        hi = len(edges) - 1
    if lo >= hi:
        return
    p = _partition(edges, lo, hi)
    quicksort(edges, lo, p - 1)
    quicksort(edges, p + 1, hi)

# =================== Kruskal ===================
def kruskal(n_vertices, edges):
    t0 = time.perf_counter()
    quicksort(edges)  # in-place
    t1 = time.perf_counter()

    dsu = DSU(n_vertices)
    mst = []
    total = 0
    for w, u, v in edges:
        if dsu.union(u, v):
            mst.append((u, v, w))
            total += w
            if len(mst) == n_vertices - 1:
                break

    t2 = time.perf_counter()
    ok = (len(mst) == n_vertices - 1)
    return {
        "custo": total if ok else None,
        "arestas_mst": mst,
        "tempo_sort_ms": (t1 - t0) * 1000.0,
        "tempo_total_ms": (t2 - t0) * 1000.0,
        "ok": ok,
    }

# =================== Geração de grafo conexo ===================
def grafo_conexo_aleatorio(n, m, peso_min=1, peso_max=100, seed=42):
    """
    Grafo não-direcionado, sem múltiplas arestas, conectado.
    Retorna lista de (w, u, v) com u < v.
    """
    assert m >= n - 1, "Para garantir conexão, m >= n-1"
    max_edges = n * (n - 1) // 2
    assert m <= max_edges, "m excede número máximo de arestas simples"

    rng = random.Random(seed)
    edges = []
    pares = set()  # guarda (u,v) com u < v

    # 1) Árvore base (garante conexão)
    perm = list(range(n))
    rng.shuffle(perm)
    for i in range(1, n):
        u, v = perm[i - 1], perm[i]
        if u > v:
            u, v = v, u
        if (u, v) not in pares:
            edges.append((rng.randint(peso_min, peso_max), u, v))
            pares.add((u, v))

    # 2) Arestas adicionais únicas
    while len(edges) < m:
        u, v = rng.randrange(n), rng.randrange(n)
        if u == v:
            continue
        if u > v:
            u, v = v, u
        if (u, v) in pares:
            continue
        edges.append((rng.randint(peso_min, peso_max), u, v))
        pares.add((u, v))

    return edges

# =================== Benchmark + CSV + Resumo ===================
def benchmark_para_csv(
    saida_csv="resultados_kruskal_quicksort.csv",
    casos=(("pequena", 50, 200), ("média", 300, 5000), ("grande", 1000, 20000)),
    repeticoes=5,
    seed_base=123,
):
    """
    Grava CSV (uma linha por execução) e imprime um RESUMO por caso.
    CSV: caso,V,E,seed,execucao,custo,tempo_sort_ms,tempo_total_ms,ok
    """
    path = Path(saida_csv)
    with path.open("w", newline="") as f:
        wcsv = csv.writer(f)
        wcsv.writerow(["caso", "V", "E", "seed", "execucao",
                       "custo", "tempo_sort_ms", "tempo_total_ms", "ok"])

        # agregadores para o resumo
        resumo = {
            nome: {"V": n, "E": m, "custos": [], "sorts": [], "totais": [], "oks": []}
            for (nome, n, m) in casos
        }

        for nome, n, m in casos:
            for k in range(repeticoes):
                seed = seed_base + k
                edges = grafo_conexo_aleatorio(n, m, seed=seed)
                res = kruskal(n, edges)

                wcsv.writerow([
                    nome, n, m, seed, k + 1,
                    res["custo"],
                    f"{res['tempo_sort_ms']:.6f}",
                    f"{res['tempo_total_ms']:.6f}",
                    int(res["ok"]),
                ])

                resumo[nome]["custos"].append(res["custo"])
                resumo[nome]["sorts"].append(res["tempo_sort_ms"])
                resumo[nome]["totais"].append(res["tempo_total_ms"])
                resumo[nome]["oks"].append(bool(res["ok"]))

    # ---- Imprime resumo “compacto” (formato da imagem 2) ----
    for nome, agg in resumo.items():
        n, m = agg["V"], agg["E"]
        custo_exemplo = agg["custos"][0]              # custo do 1º run (valor concreto)
        sort_ms = stats.mean(agg["sorts"])            # média (troque para median se quiser)
        total_ms = stats.mean(agg["totais"])
        ok_all = all(agg["oks"])

        print(f"\n== Caso {nome} ==")
        print(f"V={n}, E={m}")
        print(f"MST ok? {ok_all}")
        print(f"Custo MST: {custo_exemplo}")
        print(f"Tempo sort:  {sort_ms:.2f} ms")
        print(f"Tempo total: {total_ms:.2f} ms")

    print(f"\nCSV salvo em: {path.resolve()}")

if __name__ == "__main__":
    # Ajuste os parâmetros se quiser
    benchmark_para_csv(
        saida_csv="resultados_kruskal_quicksort.csv",
        casos=(("pequena", 50, 200), ("média", 300, 5000), ("grande", 1000, 20000)),
        repeticoes=5,
        seed_base=123,
    )
