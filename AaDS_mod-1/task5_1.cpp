#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>
#define OK (0)
#define ERROR (1)
//5_1. Реклама.
//В супермаркете решили оптимизировать показ рекламы. Известно расписание прихода и ухода покупателей (два целых числа).
// Каждому покупателю необходимо показать минимум 2 рекламы.  Рекламу можно транслировать только в целочисленные моменты
// времени. Покупатель может видеть рекламу от момента прихода до момента ухода из магазина.
// В каждый момент времени может показываться только одна реклама. Считается, что реклама показывается мгновенно.
// Если реклама показывается в момент ухода или прихода, то считается, что посетитель успел её посмотреть.
// Требуется определить минимальное число показов рекламы.

template<typename T, typename Comparator = std::less<T>>
int merge(T *arr1, size_t size1, T *arr2, size_t size2, T *dst, Comparator compare = Comparator()) {
    if (!arr1 || !arr2 || !dst) {
        std::cerr << "null-pointer was transmitted to merge()\n";
        return ERROR;
    }
    size_t ind1 = 0u;
    size_t ind2 = 0u;
    size_t indDst = 0u;
    while (ind1 < size1 && ind2 < size2) {
        dst[indDst++] = compare(arr1[ind1], arr2[ind2]) ? arr1[ind1++] : arr2[ind2++];
    }

    if (ind1 < size1) {
        std::copy_n(arr1 + ind1, size1 - ind1, dst + indDst);
    } else if (ind2 < size2) {
        std::copy_n(arr2 + ind2, size2 - ind2, dst + indDst);
    }
    return OK;
}

template<typename T, typename Comparator = std::less<T>>
int sort(T *arr, size_t size, Comparator compare = Comparator()) {
    if (!arr && size <= 1u) {
        return ERROR;
    }

    T *buf = nullptr;
    try {
        buf = new T[size];//массив по очереди будем сливать то в buf[], то в arr[]
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in run(): " << ba.what() << '\n';
        return ERROR;
    }
    // в src и dst храним указатели на массивы откуда будем данные и куда сливаем соответственно,
    // на кажной итерации внешнего цикла они меняются местами
    T *src = arr;
    T *dst = buf;
    for (size_t mergeSize = 1u; mergeSize < size; mergeSize *= 2u) {
        // mergeSize, mergeSize2 --  размеры сливаемых подмассивов первого и второго.
        // indFirstMergedAr -- индекс начала пары подмассивов
        // инвариант:  пока есть 2 массива для сливания, размер первого из пары всегда size, а размер 2го хотябы 1.
        size_t mergeSize2 = mergeSize;
        size_t indFirstMergedArr = 0u;
        for (; indFirstMergedArr < (size - mergeSize); indFirstMergedArr += mergeSize + mergeSize2) {
            //учитываем случай, что последний подмассив имеет длину меньше mergeSize
            if ((indFirstMergedArr + 2u * mergeSize) > size) {
                mergeSize2 = size - indFirstMergedArr - mergeSize;
            }
            merge(src + indFirstMergedArr, mergeSize, src + indFirstMergedArr + mergeSize, mergeSize2,
                  dst + indFirstMergedArr, compare);
        }

        //случай, если нечетное кол-во подмассивов, последний подмассив просто копируем
        if (indFirstMergedArr < size) {
            std::copy_n(src + indFirstMergedArr, size - indFirstMergedArr, dst + indFirstMergedArr);
        }
        std::swap(src, dst);
    }

    if (dst == arr) {
        std::copy_n(src, size, arr);
    }
    delete[] buf;
    return OK;
}

void testSort() {
    // int, свой компоратор, противоположно отсортированный массив
    size_t arrIntSize1 = 100u;
    int arrInt1[arrIntSize1];
    for (size_t i = 0u; i < arrIntSize1; ++i) {
        arrInt1[i] = static_cast<int>(i);
    }
    sort(arrInt1, arrIntSize1, std::greater<>());

    for (size_t i = 0u; i < arrIntSize1; ++i) {
        assert(arrInt1[i] == static_cast<int>(arrIntSize1 - i - 1u));
    }
    // массив int, компоратор по умолчанию, есть одинаковые элементы
    size_t arrIntSize2 = 100u;
    int arrInt2[arrIntSize1];
    for (size_t i = 0u; i < arrIntSize2 / 2u; ++i) {
        arrInt2[(arrIntSize2 - i - 1u)] = arrInt2[i] = static_cast<int>(i);
    }
    sort(arrInt2, arrIntSize2, std::greater<>());

    for (size_t i = 1u; i < arrIntSize2 - 1u; i += 2u) {
        assert(arrInt2[i] == arrInt2[i - 1u]);
        assert(arrInt2[i] > arrInt2[i + 1u]);
    }

    // std::string, компортатор по умолчанию
    size_t arrStrSize1 = 10u;
    std::string arrStr1[]{"aaa", "b", "www", "qwerty", "ab", "xyzxyz", "aaa", "111111111", "AAA", "#sssssssssssssssSsSSS"};
    sort(arrStr1, arrStrSize1);
    for (size_t i = 1u; i < arrStrSize1; ++i) {
        assert(arrStr1[i] >= arrStr1[i - 1u]);
    }

    // std::string, компортатор -- лябда-функция сравнение по размеру строки
    size_t arrStrSize2 = 10u;
    std::string arrStr2[]{"aaa", "b", "www", "qwerty", "ab", "xyzxyz", "aaa", "111111111", "AAA", "#sssssssssssssssSsSSS"};
    sort(arrStr2, arrStrSize2, [](const std::string &s1, const std::string &s2) { return s1.size() < s2.size(); });
    for (size_t i = 1u; i < arrStrSize2; ++i) {
        assert(arrStr2[i].size() >= arrStr2[i - 1u].size());
    }
}


struct Buyer {
    int64_t arrivalTime;
    int64_t leavingTime;
    Buyer(int64_t arrivalTime = 0u, int64_t leavingTime = 0u) : arrivalTime(arrivalTime), leavingTime(leavingTime) {}
};

std::istream &operator>>(std::istream &in, Buyer &buyer) {
    in >> buyer.arrivalTime >> buyer.leavingTime;
    return in;
}

std::ostream &operator<<(std::ostream &out, const Buyer &buyer) {
    out << "(" << buyer.arrivalTime << ", " << buyer.leavingTime << ")";
    return out;
}


bool compareBuyersByTimeInStore(const Buyer &buyer1, const Buyer &buyer2) {
    if (buyer1.leavingTime == buyer2.leavingTime) {
        return buyer1.arrivalTime < buyer2.arrivalTime;
    }
    return buyer1.leavingTime < buyer2.leavingTime;
}

void scanBuyers(Buyer *buyers, size_t buyersSize, std::istream &in = std::cin) {
    for (size_t i = 0u; i < buyersSize; ++i) {
        in >> buyers[i];
    }
}

void printBuyers(Buyer *buyers, size_t buyersSize, std::ostream &out = std::cout) {
    for (size_t i = 0u; i < buyersSize; ++i) {
        out << buyers[i];
    }
}

uint64_t getMinNumberOfAdvertising(Buyer *buyers, size_t buyersSize) {
    if(!buyers || !buyersSize) {
        return 0u;
    }
    //сортировка по возрастанию времени ухода (leavingTime), при равном времени ухода, по возрастанию времени прихода
    sort(buyers, buyersSize, compareBuyersByTimeInStore);

    uint64_t advertsCount = 2u;// хотябы один покупатель точно есть, когда будем обрабатывать первую группу
                               // с одинаковым временем прибытия, счетчик уже не сработает
    int64_t penultAdvertisingTime = buyers[0].leavingTime - 1;
    int64_t lastAdvertisingTime = buyers[0].leavingTime;

    for (size_t i = 1u; i < buyersSize - 1u; ++i) {
        // если время ухода сменится у следующего покупателя, обрабатываем этого
        if (buyers[i].leavingTime < buyers[i + 1u].leavingTime) {
            if (lastAdvertisingTime < buyers[i].arrivalTime) {
                lastAdvertisingTime = buyers[i].leavingTime;
                penultAdvertisingTime = buyers[i].leavingTime - 1u;
                advertsCount += 2u;
            } else if (penultAdvertisingTime < buyers[i].arrivalTime) {
                penultAdvertisingTime = lastAdvertisingTime;
                lastAdvertisingTime = buyers[i].leavingTime;
                ++advertsCount;
            }
        }
    }
    //обработка последнего покупателя
    if (lastAdvertisingTime < buyers[buyersSize - 1u].arrivalTime) {
        advertsCount += 2u;
    } else if (penultAdvertisingTime < buyers[buyersSize - 1u].arrivalTime) {
        ++advertsCount;
    }
    return advertsCount;
}


int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    size_t buyersSize;
    in >> buyersSize;
    if (!buyersSize) {
        out << 0;
        return OK;
    }
    Buyer *buyers = nullptr;
    try {
        buyers = new Buyer[buyersSize];
    } catch (std::bad_alloc &ba) {
        std::cerr << "bad_alloc caught in run(): " << ba.what() << '\n';
        return ERROR;
    }
    scanBuyers(buyers, buyersSize, in);
    out << getMinNumberOfAdvertising(buyers, buyersSize);
    delete[]buyers;
    return OK;
}

int main() {
    //testSort();
    return run();
}