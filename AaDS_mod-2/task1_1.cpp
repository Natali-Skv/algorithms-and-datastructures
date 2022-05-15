#include <iostream>
#include <utility>
#include <vector>
// 1_1. Для разрешения коллизий используйте квадратичное пробирование. i-ая проба
// g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.

const size_t INITIAL_CAPACITY = 8u;
const double MAX_ALPHA = 0.75;

size_t NextProbe(size_t lastProbe, size_t countProbe, size_t tableCapacity) {
    return (lastProbe + countProbe) % tableCapacity;
}


class StringHasher {
public:
    explicit StringHasher(size_t prime = 71u)
        : prime(prime) {
    }

    size_t operator()(const std::string &str, size_t tableSize) const {
        size_t hash = 0;
        for (auto c : str) {
            hash = (hash * prime + c) % tableSize;
        }
        return hash;
    }

private:
    size_t prime;
};


template<typename T, typename Hasher, typename GetterNextProbe>
class HashTable {
public:
    // я решила, что  getHash, getNextProbe, delete и emptiness -- часто rvalue-временные объеты, поэтому применила move
    // решила ограничиться этим конструктором, не создавать 2^4 перегруженных конструкторов, возможных, если каждое поле можно передать через rvalue и lvalue
    // хотя можно было сделать конструктор шаблонным и использовать forward
    explicit HashTable(const Hasher &getHash, T emptiness, T deleted, GetterNextProbe getNextProbe = NextProbe, size_t initial_capacity = INITIAL_CAPACITY)
        : amountOfElems(0u), emptiness(std::move(emptiness)), deleted(std::move(deleted)), table(initial_capacity, emptiness), getHash(getHash), getNextProbe(getNextProbe) {
    }

    bool Add(const T &key) {
        if (static_cast<double>(amountOfElems) > static_cast<double>(table.size()) * MAX_ALPHA) {
            if (!grow()) {
                return false;
            }
        }
        size_t currProbe = getHash(key, table.size());
        size_t pozFirstDeleted = 0u;
        for (size_t i = 0u; i != table.size() && table[currProbe] != emptiness; currProbe = getNextProbe(currProbe, ++i, table.size())) {
            if (table[currProbe] == key) {
                return false;
            }
            if (!pozFirstDeleted && table[currProbe] == deleted) {
                pozFirstDeleted = currProbe;
            }
        }
        size_t poz = pozFirstDeleted ? pozFirstDeleted : currProbe;
        table[poz] = key;
        ++amountOfElems;
        return true;
    }

    bool Has(const T &key) const {
        size_t currProbe = getHash(key, table.size());

        for (size_t i = 0u; i != table.size() && table[currProbe] != emptiness; currProbe = getNextProbe(currProbe, ++i, table.size())) {
            if (table[currProbe] == key) {
                return true;
            }
        }
        return false;
    }

    bool Remove(const T &key) {
        size_t currProbe = getHash(key, table.size());
        size_t i = 0u;
        for (; i != table.size() && table[currProbe] != key && table[currProbe] != emptiness; currProbe = getNextProbe(currProbe, ++i, table.size())) {
        }
        if (table[currProbe] != key) {
            return false;
        }
        table[currProbe] = deleted;
        --amountOfElems;
        return true;
    }

    void print() const {
        for (size_t i = 0u; i < table.size(); i++) {

            std::cout << "[" << i << "]: {" << table[i] << "}" << std::endl;
        }
    }

private:
    bool grow() {
        std::vector<T> newTable;
        try {
            newTable.resize(table.size() * 2u, emptiness);
        } catch (std::bad_alloc &ba) {
            std::cerr << "bad_alloc caught in grow(): " << ba.what() << '\n';
            return false;
        }

        for (size_t i = 0u; i < table.size(); ++i) {
            if (table[i] != deleted) {
                size_t currProbe = getHash(table[i], newTable.size());
                for (size_t j = 0u; newTable[currProbe] != emptiness; currProbe = getNextProbe(currProbe, ++j, newTable.size())) {
                }
                newTable[currProbe] = table[i];
            }
        }

        table = std::move(newTable);
        return true;
    }
    size_t amountOfElems;
    const T emptiness;
    const T deleted;
    std::vector<T> table;
    Hasher getHash;
    GetterNextProbe getNextProbe;
};


void run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    HashTable<std::string, StringHasher, size_t (*)(size_t, size_t, size_t)> table{
            StringHasher(), "", "D"};
    char oper = 0;
    std::string str;
    while (in >> oper) {
        switch (oper) {
            case '+': {
                in >> str;
                out << (table.Add(str) ? "OK" : "FAIL") << std::endl;
                break;
            }
            case '?': {
                in >> str;
                out << (table.Has(str) ? "OK" : "FAIL") << std::endl;
                break;
            }
            case '-': {
                in >> str;
                out << (table.Remove(str) ? "OK" : "FAIL") << std::endl;
                break;
            }
            case 'p': {
                table.print();
                break;
            }
            default:
                break;
        }
    }
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    run();
    return 0;
}
