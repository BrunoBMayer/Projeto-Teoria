import random
import time
import math
import csv
import sys

sys.setrecursionlimit(2000000)


def partition(arr, low, high):
    rand_index = random.randint(low, high)
    arr[high], arr[rand_index] = arr[rand_index], arr[high]
    pivot = arr[high]
    i = low - 1
    for j in range(low, high):
        if arr[j] <= pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    return i + 1


def quick_sort(arr, low, high):
    stack = [(low, high)]
    while stack:
        l, h = stack.pop()
        if l < h:
            p = partition(arr, l, h)
            if p + 1 < h:
                stack.append((p + 1, h))
            if l < p - 1:
                stack.append((l, p - 1))


def generate_random_array(size):
    return [random.randint(0, size * 10) for _ in range(size)]


def generate_best_case_array(size):
    return [i for i in range(size)]


def generate_worst_case_array(size):
    return [size - i for i in range(size)]


def run_tests():
    tests = [
        (1000, 'melhor'),
        (2000, 'medio'),
        (5000, 'pior')
    ]
    num_runs = 30

    print("--- Resultados QuickSort ---")
    print("Tamanho (N) | Caso       | Média (s) | Desvio Padrão (s)")
    print("-------------------------------------------------------")

    results = []

    for size, case_type in tests:
        execution_times = []

        for _ in range(num_runs):
            if case_type == 'melhor':
                arr = generate_best_case_array(size)
            elif case_type == 'pior':
                arr = generate_worst_case_array(size)
            else:
                arr = generate_random_array(size)

            arr_copy = arr.copy()
            start_time = time.time()
            quick_sort(arr_copy, 0, len(arr_copy) - 1)
            end_time = time.time()
            execution_times.append(end_time - start_time)

        avg_time = sum(execution_times) / num_runs
        std_dev = math.sqrt(
            sum((x - avg_time) ** 2 for x in execution_times) / (num_runs - 1)
        ) if num_runs > 1 else 0

        print(f"{size:<11} | {case_type:<10} | {avg_time:.6f} | {std_dev:.6f}")

        results.append({
            'N': size,
            'Caso': case_type,
            'mean_time': avg_time,
            'std_dev_time': std_dev
        })

    with open('quicksort_results_python.csv', 'w', newline='') as csvfile:
        fieldnames = ['N', 'Caso', 'mean_time', 'std_dev_time']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for result in results:
            writer.writerow(result)

    print("\nArquivo gerado: quicksort_results_python.csv")


if __name__ == "__main__":
    random.seed(time.time())
    run_tests()
    
