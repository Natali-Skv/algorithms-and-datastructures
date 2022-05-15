#include <cassert>
#include <cstring>
#include <iostream>
// ВНИМАНИЕ! Для решения данной задачи нужно использовать структуру данных стек!
//Дана последовательность, состоящая из символов '(' и ')'. Требуется определить,
// является ли данная последовательность правильной скобочной последовательностью. Длина исходной последовательности ≤ 800000.
#define START_BUF_CAPACITY 4u
#define REALLOC_SIZE 2u

template<class T>
class Stack {
public:
    Stack()
        : head(0), capacity(START_BUF_CAPACITY), buf(new T[capacity]) {
    }
    Stack(const Stack &resource) = delete;
    Stack(Stack &&resource) = delete;
    Stack &operator=(const Stack &resource) = delete;
    Stack &operator=(Stack &&resource) = delete;

    ~Stack() {
        if (buf) {
            delete[](buf);
        }
    }

    void push(T value) {

        if (capacity == head) {
            resize();
        }
        buf[head++] = value;
    }

    T pop() {
        assert(!isEmpty());
        return buf[--head];
    }

    bool isEmpty() const {
        return head == 0;
    }

private:
    size_t head;
    size_t capacity;
    T *buf;

    void resize() {
        size_t newCapacity = capacity * REALLOC_SIZE;
        auto *newBuf = new T[newCapacity];
        memcpy(newBuf, buf, capacity * sizeof(T));
        delete[] buf;
        buf = newBuf;
    }
};

bool run(std::istream &input, std::ostream &output) {
    std::string bracketSeq;
    input >> bracketSeq;
    Stack<char> stackBracketSeq;
    for (char i : bracketSeq) {
        if (i == '(') {
            stackBracketSeq.push('(');
        } else if (stackBracketSeq.isEmpty()) {
            return false;
        } else {
            stackBracketSeq.pop();
        }
    }
    if (stackBracketSeq.isEmpty()) {
        return true;
    }
    return false;
}


int main() {
    std::cout << (run(std::cin, std::cout) ? "YES" : "NO") << std::endl;
    return 0;
}
