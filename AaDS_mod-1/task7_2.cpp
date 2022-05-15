#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#define ERROR (-1)
#define OK (0)

// 7_2. LSD для long long.
// Дан массив неотрицательных целых 64-битных чисел. Количество чисел не больше 106.
// Отсортировать массив методом поразрядной сортировки LSD по байтам.

template<typename T>
void printArr(const T *arr, size_t size, std::ostream &out = std::cout);

template<typename T>
void scanArr(T *arr, size_t size, std::istream &in = std::cin);


template<typename T>
int byteSort(T *arr, size_t arrSize) {
    assert(arr != nullptr);
    assert(arrSize != 0u);

    T *tmpBuf = nullptr;
    try {
        tmpBuf = new T[arrSize];
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in sort(): " << ba.what() << '\n';
        return ERROR;
    }

    int *countBuf = nullptr;
    const size_t byteRange = 256u;
    try {
        countBuf = new int[byteRange];
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in sort(): " << ba.what() << '\n';
        delete[] tmpBuf;
        return ERROR;
    }

    const size_t bitsInByte = 8u;
    for (size_t byteNum = 0u; byteNum < sizeof(T); ++byteNum) {
        std::fill_n(countBuf, byteRange, 0u);
        for (size_t i = 0; i < arrSize; ++i) {
            ++countBuf[(arr[i] >> bitsInByte * byteNum) & 0xFF];
        }

        for (size_t i = 1u; i < byteRange; ++i) {
            countBuf[i] += countBuf[i - 1];
        }

        for (int i = static_cast<int>(arrSize) - 1; i >= 0; --i) {
            tmpBuf[--countBuf[(arr[i] >> bitsInByte * byteNum) & 0xFF]] = arr[i];
        }
        std::copy_n(tmpBuf, arrSize, arr);
    }
    delete[] countBuf;
    delete[] tmpBuf;
    return OK;
}

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    size_t arrSize;
    in >> arrSize;
    if (!arrSize) {
        return ERROR;
    }
    int64_t *arr = nullptr;
    try {
        arr = new int64_t[arrSize];
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in run(): " << ba.what() << '\n';
        return ERROR;
    }
    scanArr(arr, arrSize, in);
    byteSort(arr, arrSize);
    printArr(arr, arrSize, out);
    delete[] arr;
    return OK;
}

int main() {
    return run();
}


template<typename T>
void printArr(const T *arr, size_t size, std::ostream &out /* = std::cout */) {
    for (size_t i = 0u; i < size; ++i) {
        out << arr[i] << ' ';
    }
    out << '\n';
}

template<typename T>
void scanArr(T *arr, size_t size, std::istream &in /* = std::cin */) {
    for (size_t i = 0u; i < size; ++i) {
        in >> arr[i];
    }
}
