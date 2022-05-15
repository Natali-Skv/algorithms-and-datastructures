#include <functional>
#include <iostream>
#define ERROR (-1)
#define OK (0)
// Даны неотрицательные целые числа n,k и массив целых чисел из [0..10^9] размера n. Требуется найти k-ю порядковую
// статистику. т.е. напечатать число, которое бы стояло на позиции с индексом k (0..n-1) в отсортированном массиве.
// 6_2. Реализуйте стратегию выбора опорного элемента “медиана трёх”.
// Функцию Partition реализуйте методом прохода двумя итераторами от конца массива к началу.

template<typename T>
void printArr(const T *arr, size_t size, std::ostream &out = std::cout) {
    for (size_t i = 0u; i < size; ++i) {
        out << arr[i] << ' ';
    }
    out << '\n';
}

template<typename T>
void scanArr(T *arr, size_t size, std::istream &in = std::cin) {
    for (size_t i = 0u; i < size; ++i) {
        in >> arr[i];
    }
}

template<typename T, typename Comparator = std::less<T>>
size_t getMedianOfThree(const T *arr, int64_t ind1, int64_t ind2, int64_t ind3, Comparator cmp = Comparator()) {
    if (cmp(arr[ind1], arr[ind2]) ^ cmp(arr[ind1], arr[ind3]))
        return ind1;
    else if (cmp(arr[ind1], arr[ind2]) ^ cmp(arr[ind3], arr[ind2]))
        return ind2;
    else
        return ind3;
}

template<typename T, typename Comparator = std::less<T>>
int64_t partition(T *arr, int64_t l, int64_t r, Comparator cmp = Comparator()) {
    if (!arr || (l > r)) {
        return ERROR;
    }
    size_t pivotInd = getMedianOfThree(arr, l, (r + l) / 2, r, cmp);
    T pivot = arr[pivotInd];
    std::swap(arr[l], arr[pivotInd]);
    int64_t firstGreatOrEqInd = r;
    int64_t iter = r;
    while (iter > l) {
        while ((iter > l) && (arr[iter] < pivot)) {
            --iter;
        }
        if (iter > l) {
            std::swap(arr[firstGreatOrEqInd], arr[iter]);
            --iter;
            --firstGreatOrEqInd;
        }
    }
    std::swap(arr[firstGreatOrEqInd], arr[iter]);
    //printArr(arr + l, r  - l + 1u);
    return firstGreatOrEqInd;
}

template<typename T, typename Comparator = std::less<T>>
T kthStatistic(T *arr, int64_t k, int64_t l, int64_t r, Comparator cmp = Comparator()) {
    int pivotInd = partition(arr, l, r, cmp);
    while (pivotInd != k) {

        if (pivotInd > k) {
            r = pivotInd - 1u;
            pivotInd = partition(arr, l, r, cmp);
        } else {
            l = pivotInd + 1u;
            pivotInd = partition(arr, l, r, cmp);
        }
    }
    return arr[pivotInd];
}

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    int64_t size;
    in >> size;
    if (size <= 0) {
        return ERROR;
    }
    int64_t k;
    in >> k;
    int64_t *arr = nullptr;
    try {
        arr = new int64_t[size];
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in run(): " << ba.what() << '\n';
        return ERROR;
    }
    scanArr(arr, size, in);
    //printArr(arr, size);
    out << kthStatistic(arr, k, 0, size - 1);
    delete[] arr;
    return OK;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    return run();
}
