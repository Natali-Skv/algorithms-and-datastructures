#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#define ERROR (-1)
#define OK 0
#define BUF_CAPACITY_MULTIPLIER 2u
#define START_BUF_CAPACITY 4u
// 4_1. Жадина.
// Вовочка ест фрукты из бабушкиной корзины. В корзине лежат фрукты разной массы. Вовочка может поднять не более K грамм.
// Каждый фрукт весит не более K грамм. За раз он выбирает несколько самых тяжелых фруктов, которые может поднять
// одновременно, откусывает от каждого половину и кладет огрызки обратно в корзину. Если фрукт весит нечетное число
// грамм, он откусывает большую половину. Фрукт массы 1гр он съедает полностью.
// Определить за сколько подходов Вовочка съест все фрукты в корзине.


template<typename T, typename Comparator = std::less<T>>
class Heap {
public:
    explicit Heap(T *arr = nullptr, size_t _size = 0u, size_t _capacity = 0u)
            : capacity(std::max(_capacity, _size)), size(_size), compare(Comparator()) {
        if (!capacity) {
            return;
        }
        try {
            buf = new T[capacity];
        } catch (std::bad_alloc &ba) {
            std::cerr << "bad_alloc caught in Heap(): " << ba.what() << '\n';
            return;
        }
        buildHeap(arr, size);
    }

    Heap(const Heap &resource) = delete;

    Heap(Heap &&resource) = delete;

    Heap &operator=(const Heap &resource) = delete;

    Heap &operator=(Heap &&resource) = delete;

    ~Heap() {
            delete[] buf;
    }

    int insert(T elem) {
        if (size == capacity && resize()) {
            return ERROR;
        }
        buf[size++] = elem;
        siftUp(size - 1u);
        return OK;
    }

    T extractTop() {
        assert(!isEmpty());
        std::swap(buf[0u], buf[--size]);
        siftDown(0u);
        return buf[size];
    }

    T peekTop() const {
        assert(!isEmpty());
        return buf[0];
    }

    bool isEmpty() const {
        return (size == 0u);
    }

    void printBuf(std::ostream &out = std::cout, char separator = ' ') const {
        for(size_t i = 0u; i<size; ++i) {
            out<<buf[i]<< separator;
        }
        out<<'\n';
    }

private:
    size_t capacity;
    size_t size;
    T *buf;
    Comparator compare;

    int resize() {
        if (!capacity) {
            try {
                buf = new T[START_BUF_CAPACITY];
                capacity = START_BUF_CAPACITY;
            } catch (std::bad_alloc &ba) {
                std::cerr << "bad_alloc caught in Heap(): " << ba.what() << '\n';
                return ERROR;
            }
            return OK;
        }
        T *newBuf = nullptr;
        try {
            newBuf = new T[capacity * BUF_CAPACITY_MULTIPLIER];
            capacity *= BUF_CAPACITY_MULTIPLIER;
        } catch (std::bad_alloc &ba) {
            std::cerr << "bad_alloc caught in Heap(): " << ba.what() << '\n';
            return ERROR;
        }

        std::copy_n(buf, size, newBuf);
        delete[]buf;
        buf = newBuf;
        return OK;
    }

    int buildHeap(T *arr, size_t arrSize) const {
        if (arrSize > capacity) {
            return ERROR;
        }
        std::copy_n(arr, arrSize, buf);

        for (int i = size / 2 - 1; i >= 0; --i) {
            siftDown(i);
        }
        return OK;
    }

    void siftUp(int ind) const {
        while (ind > 0) {
            int parent = (ind - 1) / 2;
            if (compare(buf[parent], buf[ind]))
                return;
            std::swap(buf[parent], buf[ind]);
            ind = parent;
        }
    }


    void siftDown(size_t ind) const {
        size_t right_child_ind;
        while ((right_child_ind = ind * 2u + 2u) < size) {
            size_t max_child_ind = (compare(buf[right_child_ind - 1u], buf[right_child_ind])) ? right_child_ind -
                                                                                                1u : right_child_ind;
            if (compare(buf[max_child_ind], buf[ind])) {
                std::swap(buf[max_child_ind], buf[ind]);
                ind = max_child_ind;
            } else {
                return;
            }
        }

        if (right_child_ind - 1u < size && compare(buf[right_child_ind - 1u], buf[ind])) {
            std::swap(buf[right_child_ind - 1u], buf[ind]);
        }
    }

};


void test_heap(std::ostream &out) {
    //min heap of int
    Heap<int> minHeapInsert;
    assert(minHeapInsert.isEmpty());
    int elems_num = 10;
    int curr = 0;
    while (elems_num--) {
        minHeapInsert.insert(curr++);
    }
    minHeapInsert.printBuf(out);
    int last = 0u;
    if (!minHeapInsert.isEmpty()) {
        last = minHeapInsert.extractTop();
        out<<last<<' ';
    }
    while (!minHeapInsert.isEmpty()) {
        assert(last <= (curr = minHeapInsert.extractTop()));
        last = curr;
        out<<last<<' ';
    }
    out<<'\n';

    //  свой компоратор, построение из массива
    int arr[170];
    for(int i = -70; i<100; ++i) {
        arr[i + 70] = i;
    }
    Heap<int, std::greater<>> maxHeapArr{ arr, 170};
    assert(!maxHeapArr.isEmpty());
    maxHeapArr.printBuf(out);
    elems_num = 170;
    curr = -70;
    while (elems_num--) {
        maxHeapArr.insert(curr++);
    }
    maxHeapArr.printBuf(out);
    last = 0u;
    if (!maxHeapArr.isEmpty()) {
        last = maxHeapArr.extractTop();
        out<<last<<' ';
    }
    while (!maxHeapArr.isEmpty()) {
        assert(last >= (curr = maxHeapArr.extractTop()));
        last = curr;
        out<<last<<' ';
    }
    out<<'\n';

    //string
    std::string strArr[]{"aaa","b","www","qwerty","ab","xyzxyz", "aaa", "111111111", "AAA"};
    Heap<std::string, std::greater<>> maxHeapString{strArr, 9u };
    maxHeapString.insert("12222222");
    maxHeapString.printBuf(out);
    std::string lastStr;
    if (!maxHeapString.isEmpty()) {
        lastStr = maxHeapString.extractTop();
        out<<lastStr<<' ';
    }
    std::string currStr;
    while (!maxHeapString.isEmpty()) {
        currStr = maxHeapString.peekTop();
        assert(currStr == maxHeapString.extractTop());
        assert(lastStr >= currStr);
        lastStr = currStr;
        out<<lastStr<<' ';
    }
    out<<'\n';
}

void run(std::istream &in, std::ostream &out) {
    size_t fruitsNum;
    in >> fruitsNum;
    auto fruitsBuf = new size_t[fruitsNum];
    for (size_t i = 0; i < fruitsNum; ++i) {
        in >> fruitsBuf[i];
    }
    size_t vovasCapacity;
    in>>vovasCapacity;
    Heap<size_t,std::greater<>> maxHeapFruits(fruitsBuf, fruitsNum);
    size_t approaches = 0;

    while(!maxHeapFruits.isEmpty()) {
        size_t fruitsNumInApproach = 0u;
        size_t massTakenFruits = 0u;
        while(!maxHeapFruits.isEmpty() && (massTakenFruits += maxHeapFruits.peekTop()) <= vovasCapacity) {
            size_t currFruit = maxHeapFruits.extractTop();
            fruitsBuf[fruitsNumInApproach++] = currFruit;
        }
        ++approaches;
        for(size_t i = 0u; i<fruitsNumInApproach && fruitsBuf[i] > 1u; ++i)
        maxHeapFruits.insert(fruitsBuf[i]/2u);
    }
    out<<approaches;
    delete[]fruitsBuf;
}


int main() {
    //test_heap(std::cout);
    run(std::cin, std::cout);
    return 0;
}
