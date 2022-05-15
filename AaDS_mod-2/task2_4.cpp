#include <iostream>
#include <queue>
// Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.
// Требуется построить бинарное дерево, заданное наивным порядком вставки.
// Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
// 2_4. Выведите элементы в порядке level-order (по слоям, “в ширину”).

template<class T>
struct Node {
    T key;
    Node *left;
    Node *right;
    explicit Node(const T &value) : key{value}, left{nullptr}, right{nullptr} {}
    explicit Node(T &&value) : key(std::move(value)), left(nullptr), right(nullptr) {}
};

template<typename T, typename Comparator = std::less<T>>
class BinSearchTree {
public:
    explicit BinSearchTree(Comparator Compare = Comparator()) : root(nullptr), Compare(Compare) {}
    BinSearchTree(const BinSearchTree<T> &) = delete;
    BinSearchTree(BinSearchTree<T> &&) = delete;
    BinSearchTree<T> &operator=(const BinSearchTree<T> &) = delete;
    BinSearchTree<T> &operator=(BinSearchTree<T> &&) = delete;
    ~BinSearchTree() {
        // почти копипаста с TraverseBFS, так как тут для удаления нужен указатель на узел Node, а в  TraverseBFS() логично
        // при обработке узла обрабатывать сами данные типа Т, а указатели на детей оставить недоступными
        // т.е. было невозможно использовать например  'TraverseBFS<void (*)(Node)>([](Node* node) { delete node; });' ,
        // так как для удаления нужен тип Node *, а в строке 'visit(node->key);' из реализации TraverseBFS мы передаем тип Т
        if (!root) {
            return;
        }
        std::queue<Node<T> *> nodeQueue;
        nodeQueue.push(root);
        while (!nodeQueue.empty()) {
            Node<T> *node = nodeQueue.front();
            nodeQueue.pop();
            if (node->left) {
                nodeQueue.push(node->left);
            }
            if (node->right) {
                nodeQueue.push(node->right);
            }
            delete node;
        }
    }
    void Add(T key) {
        if (!root) {
            root = new Node<T>(std::move(key));
            return;
        }
        Node<T> *parent = root;
        bool compereResult = Compare(key, parent->key);
        while (compereResult ? parent->left : parent->right) {
            parent = compereResult ? parent->left : parent->right;
            compereResult = Compare(key, parent->key);
        }
        (compereResult ? parent->left : parent->right) = new Node<T>(std::move(key));
    }

    template<class Visitor>
    void TraverseBFS(Visitor visit) {
        if (!root) {
            return;
        }
        std::queue<Node<T> *> nodeQueue;
        nodeQueue.push(root);
        while (!nodeQueue.empty()) {
            Node<T> *node = nodeQueue.front();
            nodeQueue.pop();
            if (node->left) {
                nodeQueue.push(node->left);
            }
            if (node->right) {
                nodeQueue.push(node->right);
            }
            visit(node->key);
        }
    }

private:
    Node<T> *root;
    Comparator Compare;
};

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    size_t n = 0u;
    in >> n;
    BinSearchTree<int> tree;
    while (n--) {
        int key;
        in >> key;
        tree.Add(key);
    }
    tree.TraverseBFS([&out](int key) mutable -> void { out << key << ' '; });
    return 0;
}


int main() {

    return run();
}