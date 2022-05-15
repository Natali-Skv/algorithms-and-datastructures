#include <cassert>
#include <iostream>
// 2_4. Дан отсортированный массив различных целых чисел A[0..n-1] и массив целых чисел B[0..m-1].
// Для каждого элемента массива B[i] найдите минимальный индекс элемента массива A[k], ближайшего
// по значению к B[i].
// Требования: Время работы поиска для каждого элемента B[i]: O(log(k)).
// Внимание! В этой задаче для каждого B[i] сначала нужно определить диапазон для бинарного поиска
// размером порядка k с помощью экспоненциального поиска, а потом уже в нем делать бинарный поиск.


// возврат левой границы полуинтервала [left; left * 2 + 1) или [left; n) (если (2*left + 1)>n)
// на котором надо искать элемент key в mass[]
int ExpSearchLeftBound(const int *arr, int size, int key) {
    assert("null pointer transmitted" && arr);
    int left = 1;
    while ((left < size) && (arr[left] <= key)) {
        left <<= 1;
    }
    return left >> 1;
}

// Возвращает позицию ближайшего по значению элемента на диапозоне [0, size).
int FindTwin(const int *arr, int size, int key) {
    assert("null pointer transmitted" && arr);
    int first = 0;
    int last = size;
    while (first < last) {
        int mid = (first + last) >> 1;
        if (arr[mid] < key) {
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    if ((first == size) || (first) && (key - arr[first - 1]) <= (arr[first] - key)) {
        --first;
    }
    return first;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int sizeA = 0;
    std::cin >> sizeA;
    assert("size A[] must be positive and less than 110'001" && (sizeA >= 0) && (sizeA <= 110000));
    int *a = new int[sizeA];
    for (int i = 0; i < sizeA; ++i) {
        std::cin >> a[i];
    }
    int sizeB = 0;
    std::cin >> sizeB;
    assert("size B[] must be positive and less than 1'001" && (sizeB >= 0) && (sizeB <= 1000));
    int *b = new int[sizeB];
    for (int i = 0; i < sizeB; ++i) {
        std::cin >> b[i];
    }

    int insertIndex = 0;
    int left = 0;
    for (int i = 0; i < sizeB; ++i) {
        //если  b[i] > b[i-1], то так как массив А[] отсортирован, то мы можем не искать близнеца левее insertIndex для b[i-1]
        if ((i > 0) && b[i] > b[i - 1]) {
            left = insertIndex + ExpSearchLeftBound(a + insertIndex, sizeA - insertIndex, b[i]);
        } else {
            left = ExpSearchLeftBound(a, sizeA, b[i]);
        }
        //последний индекс на котором надо искать элемент, это (left + 1) => (если учитывать только часть массива с A[left]) размер массива = (left + 2)
        //(left<<1) + 2 -- размер массива, если считать с А[0]
        insertIndex = left + FindTwin(a + left, (((left << 1) + 2) < (sizeA)) ? (left + 2) : (sizeA - left), b[i]);
        std::cout << insertIndex << ' ';
    }

    delete[](a);
    delete[](b);
}
