#include <cassert>
#include <cstring>
#include <iostream>
#define EOQ 0u
#define QEMPTY -1
#define DEQUEUE 2
#define ENQUEUE 3

class Stack {
public:
    Stack() : buf(nullptr), capacity(EOQ), size(EOQ), head(EOQ), tail(EOQ) {
    }
    Stack(const Stack &q) = delete;
    Stack(Stack &&q) = delete;

    Stack &operator=(Stack &&q) = delete;

    ~Stack() {
        if (buf) {
            delete[](buf);
        }
    }

    bool Enqueue(int64_t elem) {
        if (capacity == EOQ) {
            try {
                buf = new int64_t[1];
                capacity = 1;
                head = tail = 0;
                size = 0;
            } catch (std::bad_alloc) {
                return false;
            }
        } else if ((size == capacity) && !resize()) {
            return false;
        }
        tail = (++tail) % capacity;
        buf[tail] = elem;
        ++size;
        return true;
    }

    int64_t Dequeue() {
        assert(!IsEmpty());
        --size;
        int64_t data = buf[head];
        head = (++head) % capacity;
        return data;
    }

    bool IsEmpty() const {
        return (size == EOQ);
    }

private:
    int64_t *buf;
    size_t capacity;
    size_t size;
    size_t head;
    size_t tail;

    bool resize() {
        int64_t *tmpBuf = nullptr;
        size_t tmpCapacity = capacity;
        try {
            tmpBuf = new int64_t[tmpCapacity * 2u];
            tmpCapacity *= 2u;
        } catch (std::bad_alloc &badAlloc2) {//если не удалось выделить в 2 раза больше, пробуем выделить в 1,5 раза больше
            try {
                tmpBuf = new int64_t[(tmpCapacity * 3) / 2];
                tmpCapacity = (tmpCapacity * 3) / 2;
            } catch (std::bad_alloc &badAlloc1_5) {
                return false;
            }
        }
        if (head < tail) {
            memcpy(tmpBuf, buf, capacity * sizeof(int64_t));
        } else {
            memcpy(tmpBuf, buf + head, (capacity - head) * sizeof(int64_t));
            memcpy(tmpBuf + capacity - head, buf, (tail + 1) * sizeof(int64_t));
            head = 0u;
            tail = capacity - 1u;
        }
        delete[](buf);
        capacity = tmpCapacity;
        buf = tmpBuf;
        return true;
    }
};

void run(std::istream &in, std::ostream &out) {
    Stack queue;
    int cmd_number = 0;
    in >> cmd_number;
    for (int i = 1; i <= cmd_number; i++) {
        int oper = 0, value = 0;
        in >> oper >> value;
        switch (oper) {
            case DEQUEUE: {
                int qValue = 0;
                if (!queue.IsEmpty()) {//так как возврат -1 не вписывается в логику метода Deque,
                                       // а продиктован условием задачи, логично делать проверку тут, вне метода
                    qValue = queue.Dequeue();
                } else {
                    qValue = QEMPTY;
                }
                if (qValue != value) {
                    out << "NO" << std::endl;
                    return;
                }
                break;
            }
            case ENQUEUE: {
                queue.Enqueue(value);
                break;
            }
            default:
                out << "wrong input, command №" << i;
        }
    }
    out << "YES" << std::endl;
}

int main(int argc, const char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    run(std::cin, std::cout);
    return 0;
}