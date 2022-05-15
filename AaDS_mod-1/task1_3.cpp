#include <cassert>
#include <iostream>

// Даны два строго возрастающих массива целых чисел A[1..n) и B[0..m) и число k.
// Найти количество таких пар индексов (i, j), что A[i] + B[j] = k.
// n, m ≤ 100000.
// Указание. Обходите массив B от конца к началу.
// Требования:  Время работы O(n + m).

int PairsNumberWithGivenSum(const int *a, int sizeA, const int *b, int sizeB, int k) {
    assert("null pointer transmitted" && a && b);
    int iA = 0;
    int iB = sizeB - 1;
    int num = 0;
    while ((iA < sizeA) && (iB >= 0)) {
        int sum = a[iA] + b[iB];
        if (sum == k) {
            ++num;
            ++iA;
            --iB;
        } else {
            (sum < k) ? ++iA : --iB;
        }
    }
    return num;
}

int main() {
    int sizeA = 0;
    std::cin >> sizeA;
    assert("size A[] must be positive and less than 100'000" && (sizeA >= 0) && (sizeA <= 100000));
    int *a = new int[sizeA];
    for (int i = 0; i < sizeA; ++i) {
        std::cin >> a[i];
    }
    int sizeB = 0;
    std::cin >> sizeB;
    assert("size B[] must be positive and less than 100'000" && (sizeB >= 0) && (sizeB <= 100000));
    int *b = new int[sizeB];
    for (int i = 0; i < sizeB; ++i) {
        std::cin >> b[i];
    }
    int k = 0;
    std::cin >> k;
    std::cout << PairsNumberWithGivenSum(a, sizeA, b, sizeB, k);
    delete[](a);
    delete[](b);
}
