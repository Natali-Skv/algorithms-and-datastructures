#include <iostream>
#include <queue>
// 3_2. Вычислить количество узлов в самом широком слое декартового дерева и количество узлов в самом широком слое
// наивного дерева поиска. Вывести их разницу. Разница может быть отрицательна.



template<typename T, typename Comparator = std::less<T>>
class Treap {
public:
    explicit Treap(Comparator Compare = Comparator()) : root{nullptr}, Compare{Compare} {}
    Treap(const Treap<T> &) = delete;
    Treap(Treap<T> &&) = delete;
    Treap<T> &operator=(const Treap<T> &) = delete;
    Treap<T> &operator=(Treap<T> &&) = delete;
    ~Treap() {
        if (!root) {
            return;
        }
        std::queue<TreapNode *> nodeQueue;
        nodeQueue.push(root);
        while (!nodeQueue.empty()) {
            TreapNode *node = nodeQueue.front();
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

    void Add(T key, int priority) {
        if (!root) {
            root = new TreapNode(std::move(key), priority);
            return;
        }
        // parent -- указатель на родительский элемент для вставляемого
        TreapNode *parent = root;
        TreapNode *iter = root;
        //
        while ((iter = Compare(key, iter->key) ? iter->left : iter->right) && (iter->priority >= priority)) {
            parent = iter;
        }
        auto *newNode = new TreapNode(std::move(key), priority);
        if (root->priority < newNode->priority) {
            Split(root, newNode->key, newNode->left, newNode->right);
            root = newNode;
            return;
        }
        TreapNode *&newNodePoz = Compare(newNode->key, parent->key) ? parent->left : parent->right;
        Split(newNodePoz, newNode->key, newNode->left, newNode->right);
        newNodePoz = newNode;
    }

    size_t MaxLayerWidth() const {
        if (!root) {
            return 0u;
        }
        std::queue<TreapNode *> nodeQueue;
        nodeQueue.push(root);
        size_t reverseLayerIter = 1u;
        size_t maxLayerWidth = 1u;
        while (!nodeQueue.empty()) {
            TreapNode *node = nodeQueue.front();
            nodeQueue.pop();
            --reverseLayerIter;
            if (node->left) {
                nodeQueue.push(node->left);
            }
            if (node->right) {
                nodeQueue.push(node->right);
            }
            if (!reverseLayerIter) {
                reverseLayerIter = nodeQueue.size();
                if (maxLayerWidth < reverseLayerIter) {
                    maxLayerWidth = reverseLayerIter;
                }
            }
        }
        return maxLayerWidth;
    }

private:
    struct TreapNode {
        T key;
        int priority;
        TreapNode *left;
        TreapNode *right;
        explicit TreapNode(const T &key, int priority) : key{key}, priority{priority}, left{nullptr}, right{nullptr} {}
        explicit TreapNode(T &&key, int priority) : key{std::move(key)}, priority{priority}, left{nullptr}, right{nullptr} {}
    };

    // Разрезание декартового дерева по ключу.
    void Split(TreapNode *currentNode, const T &separator, TreapNode *&left,
               TreapNode *&right) {
        if (!currentNode) {
            left = nullptr;
            right = nullptr;
        } else if (Compare(currentNode->key, separator)) {// <=
            Split(currentNode->right, separator, currentNode->right, right);
            left = currentNode;
        } else {
            Split(currentNode->left, separator, left, currentNode->left);
            right = currentNode;
        }
    }

    TreapNode *root;
    Comparator Compare;
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
        if (!root) {
            return;
        }
        std::queue<Node *> nodeQueue;
        nodeQueue.push(root);
        while (!nodeQueue.empty()) {
            Node *node = nodeQueue.front();
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
            root = new Node(std::move(key));
            return;
        }
        Node *parent = root;
        bool compereResult = Compare(key, parent->key);
        while (compereResult ? parent->left : parent->right) {
            parent = compereResult ? parent->left : parent->right;
            compereResult = Compare(key, parent->key);
        }
        (compereResult ? parent->left : parent->right) = new Node(std::move(key));
    }

    size_t MaxLayerWidth() const {
        if (!root) {
            return 0u;
        }
        std::queue<Node *> nodeQueue;
        nodeQueue.push(root);
        size_t reverseLayerIter = 1u;
        size_t maxLayerWidth = 1u;
        while (!nodeQueue.empty()) {
            Node *node = nodeQueue.front();
            nodeQueue.pop();
            --reverseLayerIter;
            if (node->left) {
                nodeQueue.push(node->left);
            }
            if (node->right) {
                nodeQueue.push(node->right);
            }
            if (!reverseLayerIter) {
                reverseLayerIter = nodeQueue.size();
                if (maxLayerWidth < reverseLayerIter) {
                    maxLayerWidth = reverseLayerIter;
                }
            }
        }
        return maxLayerWidth;
    }

private:
    struct Node {
        T key;
        Node *left;
        Node *right;
        explicit Node(const T &value) : key{value}, left{nullptr}, right{nullptr} {}
        explicit Node(T &&value) : key(std::move(value)), left(nullptr), right(nullptr) {}
    };

    Node *root;
    Comparator Compare;
};

int run(std::istream &in, std::ostream &out) {
    size_t n = 0u;
    in >> n;
    Treap<int> treap;
    BinSearchTree<int> binSearchTree;
    while (n--) {
        int key, priority;
        in >> key >> priority;
        treap.Add(key, priority);
        binSearchTree.Add(key);
    }
    out << static_cast<int>(treap.MaxLayerWidth() - binSearchTree.MaxLayerWidth());
    return 0;
}

int main() {
    run(std::cin, std::cout);
    return 0;
}
