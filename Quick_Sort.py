import time
import math
import random
import sys

# AUMENTA O LIMITE DE RECURSÃO
sys.setrecursionlimit(1000000)

def troca(a, b):
    return b, a

def particiona(v, inicio, fim, mode):
    if mode == 2:
        pivo = v[inicio]
    else:
        pivo = v[(inicio + fim) // 2]

    i = inicio
    j = fim

    while i <= j:
        while v[i] < pivo:
            i += 1
        while v[j] > pivo:
            j -= 1
        if i <= j:
            v[i], v[j] = troca(v[i], v[j])
            i += 1
            j -= 1
    return i

def quicksort(v, inicio, fim, mode):
    if inicio < fim:
        meio = particiona(v, inicio, fim, mode)
        quicksort(v, inicio, meio - 1, mode)
        quicksort(v, meio, fim, mode)

def gerar_melhor_caso(v, n):
    for i in range(n):
        v[i] = i

def gerar_pior_caso(v, n):
    for i in range(n):
        v[i] = n - i - 1

def gerar_caso_medio(v, n):
    random.seed(42)
    for i in range(n):
        v[i] = random.randint(0, 1000000)

def repeticoes(modo, n, rep, tamanho_index, total_tamanhos, caso_nome):
    tempos = [0.0] * rep
    soma = 0.0

    print(f"\n📊 [{tamanho_index+1}/{total_tamanhos}] {caso_nome} - n={n}")

    for k in range(rep):
        arr = [0] * n

        if modo == 1:
            gerar_melhor_caso(arr, n)
        elif modo == 2:
            gerar_pior_caso(arr, n)
        else:
            gerar_caso_medio(arr, n)

        start = time.process_time()
        quicksort(arr, 0, n - 1, modo)
        end = time.process_time()

        tempos[k] = end - start
        soma += tempos[k]

        # Mostra progresso a cada repetição
        progresso = (k + 1) / rep * 100
        print(f"   🔄 {k+1}/{rep} ({progresso:.1f}%) - Tempo: {tempos[k]:.6f}s")

    media = soma / rep

    soma_quadrados = 0.0
    for i in range(rep):
        soma_quadrados += (tempos[i] - media) * (tempos[i] - media)

    desvio = math.sqrt(soma_quadrados / rep)
    total = soma

    resp = [0.0] * 3
    resp[0] = media
    resp[1] = desvio
    resp[2] = total

    print(f"   ✅ Finalizado - Média: {media:.6f}s")

    return resp

def gerar_csv(tamanhos, qtde_tamanhos, rep, nome_arquivo):
    print("🚀 INICIANDO TESTES DO QUICKSORT")
    print(f"📁 Arquivo de saída: {nome_arquivo}")
    print(f"🔢 Tamanhos: {tamanhos}")
    print(f"🔄 Repetições: {rep}")
    print(f"📈 Progresso:\n")

    csv = open(nome_arquivo, "w", encoding="utf-8")
    csv.write("TABELAS DE RESULTADOS DO QUICKSORT\n\n")

    for i in range(qtde_tamanhos):
        atual = tamanhos[i]

        print(f"========================================")
        print(f"🎯 VETOR TAMANHO: {atual}")
        print(f"========================================")

        # Melhor caso
        resp = repeticoes(1, atual, rep, i, qtde_tamanhos, "MELHOR CASO")
        csv.write(f"===== MELHOR CASO (n={atual}) =====\n")
        csv.write("Repeticoes,Media,Desvio,Tempo_total\n")
        csv.write(f"{rep},{resp[0]:.6f},{resp[1]:.6f},{resp[2]:.6f}\n\n")

        # Pior caso
        resp = repeticoes(2, atual, rep, i, qtde_tamanhos, "PIOR CASO")
        csv.write(f"===== PIOR CASO (n={atual}) =====\n")
        csv.write("Repeticoes,Media,Desvio,Tempo_total\n")
        csv.write(f"{rep},{resp[0]:.6f},{resp[1]:.6f},{resp[2]:.6f}\n\n")

        # Caso médio
        resp = repeticoes(3, atual, rep, i, qtde_tamanhos, "CASO MÉDIO")
        csv.write(f"===== CASO MEDIO (n={atual}) =====\n")
        csv.write("Repeticoes,Media,Desvio,Tempo_total\n")
        csv.write(f"{rep},{resp[0]:.6f},{resp[1]:.6f},{resp[2]:.6f}\n\n")

    csv.close()
    print(f"\n🎉 Arquivo '{nome_arquivo}' gerado com sucesso!")
    print("💾 Todos os testes concluídos!")

def main():
    n = [1000, 10000, 50000]
    gerar_csv(n, 3, 20, "resultados_python.csv")

if __name__ == "__main__":
    main()