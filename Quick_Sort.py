import time
import math
import random

def troca(a, b):
    aux = a
    a = b
    b = aux
    return a, b

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

def gerar_melhor_caso(arr, n):
    for i in range(n):
        arr[i] = i

def gerar_pior_caso(arr, n):
    for i in range(n):
        arr[i] = i

def repeticoes(modo, n, repeticoes_):
    tempos = [0.0] * repeticoes_
    soma = 0.0

    for k in range(repeticoes_):
        arr = [0] * n

        if modo == 1:
            gerar_melhor_caso(arr, n)
        elif modo == 2:
            gerar_pior_caso(arr, n)
        else:
            
            random.seed(time.time())
        for i in range(n):
            arr[i] = random.randint(0, 1000000 - 1)

        start = time.process_time()
        quicksort(arr, 0, n - 1, modo)
        end = time.process_time()

        tempos[k] = (end - start)
        soma += tempos[k]

    media = soma / repeticoes_

    soma_quadrados = 0.0
    for i in range(repeticoes_):
        soma_quadrados += (tempos[i] - media) * (tempos[i] - media)

    desvio = math.sqrt(soma_quadrados / repeticoes_)

    resp = [0.0] * 3
    resp[0] = media
    resp[1] = desvio
    resp[2] = soma

    return resp

def gerar_csv(tamanhos, qtde_tamanhos, rep, nome_arquivo):
    try:
        csv = open(nome_arquivo, "w", encoding="utf-8")
    except OSError:
        print("Erro ao criar CSV!")
        return

    csv.write("TABELAS DE RESULTADOS DO QUICKSORT\n\n")

    for i in range(qtde_tamanhos):
        atual = tamanhos[i]

        # Melhor caso
        csv.write(f"===== MELHOR CASO (n={atual}) =====\n")
        csv.write("Repeticoes, Media, Desvio, Tempo total\n")

        resp = repeticoes(1, atual, rep)
        csv.write(f"{rep}, {resp[0]:.6f}, {resp[1]:.6f}, {resp[2]:.6f}\n")

        csv.write("\n")

        # Pior caso
        csv.write(f"===== PIOR CASO (n={atual}) =====\n")
        csv.write("Repeticoes,Media,Desvio,Tempo total\n")

        resp = repeticoes(2, atual, rep)
        csv.write(f"{rep},{resp[0]:.6f},{resp[1]:.6f},{resp[2]:.6f}\n")
        print(f"Pior caso: n={atual}, repetição {rep} concluída")

        csv.write("\n")

        # Caso médio
        csv.write(f"===== CASO MEDIO (n={atual}) =====\n")
        csv.write("Repeticoes,Media,Desvio,Tempo total\n")

        resp = repeticoes(3, atual, rep)
        csv.write(f"{rep},{resp[0]:.6f},{resp[1]:.6f},{resp[2]:.6f}\n")

        csv.write("\n")

    csv.close()
    print(f"Arquivo '{nome_arquivo}' gerado com sucesso!")

def main():
    n = [1000, 10000, 50000]
    gerar_csv(n, 3, 20, "resultados.csv")

if __name__ == "__main__":
    main()

