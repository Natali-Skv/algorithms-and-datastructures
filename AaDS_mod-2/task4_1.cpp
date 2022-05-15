#include <iostream>

// 4_1. Солдаты. В одной военной части решили построить в одну шеренгу по росту. Т.к. часть была далеко не образцовая,
// то солдаты часто приходили не вовремя, а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
// Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту – сначала самые высокие,
// а в конце – самые низкие. За расстановку солдат отвечал прапорщик, который заметил интересную особенность –
// все солдаты в части разного роста. Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
// а именно для каждого приходящего солдата указывать, перед каким солдатом в строе он должен становится.

// 0 -- first==second;   -1 -- first<second;   1 -- first>second
template<typename T>
class DefaultComparator {
public:
    int operator()(const T &first, const T &second) const {
        if (first < second) {
            return -1;
        }
        if (first > second) {
            return 1;
        }
        return 0;
    }
};

// 0 -- first==second;   -1 -- first>second;   1 -- first<second
template<typename T>
class InvertedComparator {
public:
    int operator()(const T &first, const T &second) const {
        if (first > second) {
            return -1;
        }
        if (first < second) {
            return 1;
        }
        return 0;
    }
};

template<typename T, typename Comparator = DefaultComparator<T>>
class AvlTree {
public:
    struct Node {
        explicit Node(const T &key)
            : key(key), height(1u), numOfElems(1u), left(nullptr), right(nullptr) {
        }
        explicit Node(T &&key)
            : key(std::move(key)), height(1u), numOfElems(1u), left(nullptr), right(nullptr) {
        }
        T key;
        uint8_t height;
        size_t numOfElems;
        Node *left;
        Node *right;
    };

    explicit AvlTree(Comparator Compare = Comparator())
        : root(nullptr), Compare(Compare) {
    }
    ~AvlTree() {
        DestroyTree(root);
    }

    void Add(const T &key) {
        root = AddInternal(root, key);
    }
    void Add(T &&key) {
        root = AddInternal(root, std::move(key));
    }

    void DeletePos(const T &key) {
        root = DeletePosInternal(root, key);
    }

    int GetPosAtSorted(const T &key) {
        size_t pos = 0;
        Node *iter = root;
        while (iter) {
            if (Compare(key, iter->key) < 0) {
                iter = iter->left;
            } else if (Compare(key, iter->key) > 0) {
                pos += GetNumOfElems(iter->left) + 1;
                iter = iter->right;
            } else {
                return static_cast<int>(pos + GetNumOfElems(iter->left));
            }
        }
        return -1;
    }


private:
    Node *AddInternal(Node *node, const T &key) {
        if (!node)
            return new Node(key);
        if (Compare(key, node->key) > 0)
            node->right = AddInternal(node->right, key);
        else
            node->left = AddInternal(node->left, key);
        return DoBalance(node);
    }

    Node *AddInternal(Node *node, T &&key) {
        if (!node)
            return new Node(std::move(key));
        if (Compare(key, node->key) > 0)
            node->right = AddInternal(node->right, std::move(key));
        else
            node->left = AddInternal(node->left, std::move(key));
        return DoBalance(node);
    }

    Node *DeletePosInternal(Node *node, size_t pos) {
        if (!node)
            return nullptr;
        if (GetNumOfElems(node->left) < pos)
            node->right = DeletePosInternal(node->right, (pos - GetNumOfElems(node->left) - 1));
        else if (GetNumOfElems(node->left) > pos)
            node->left = DeletePosInternal(node->left, pos);
        else {
            Node *left = node->left;
            Node *right = node->right;
            delete node;

            if (!right) {
                return left;
            }
            if (!left) {
                return right;
            }

            if (right->height > left->height) {
                Node *minNode = nullptr;
                right = FindAndRemoveMin(right, minNode);
                minNode->right = right;
                minNode->left = left;
                return DoBalance(minNode);
            }

            Node *maxNode = nullptr;
            left = FindAndRemoveMax(left, maxNode);
            maxNode->left = left;
            maxNode->right = right;
            return DoBalance(maxNode);
        }
        return DoBalance(node);
    }

    Node *FindAndRemoveMin(Node *node, Node *&minNode) {
        if (!node) {
            minNode = nullptr;
            return nullptr;
        }
        if (!node->left) {
            minNode = node;
            Node *tmp = node->right;
            node->right = nullptr;
            FixNumOfElems(node);
            return tmp;
        }
        node->left = FindAndRemoveMin(node->left, minNode);
        return DoBalance(node);
    }

    Node *FindAndRemoveMax(Node *node, Node *&maxNode) {
        if (!node) {
            maxNode = nullptr;
            return nullptr;
        }
        if (!node->right) {
            maxNode = node;
            Node *tmp = node->left;
            node->left = nullptr;
            FixNumOfElems(node);
            return tmp;
        }
        node->right = FindAndRemoveMax(node->right, maxNode);
        return DoBalance(node);
    }

    Node *DoBalance(Node *node) {
        FixHeight(node);
        FixNumOfElems(node);
        switch (GetBalance(node)) {
            case 2: {
                if (GetBalance(node->right) < 0)
                    node->right = RotateRight(node->right);
                return RotateLeft(node);
            }
            case -2: {
                if (GetBalance(node->left) > 0)
                    node->left = RotateLeft(node->left);
                return RotateRight(node);
            }
            default:
                return node;
        }
    }

    Node *RotateLeft(Node *node) {
        Node *tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;
        FixHeight(node);
        FixHeight(tmp);
        FixNumOfElems(node);
        FixNumOfElems(tmp);
        return tmp;
    }

    Node *RotateRight(Node *node) {
        Node *tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;
        FixHeight(node);
        FixHeight(tmp);
        FixNumOfElems(node);
        FixNumOfElems(tmp);
        return tmp;
    }

    uint8_t GetHeight(Node *node) {
        return node ? node->height : 0u;
    }

    void FixHeight(Node *node) {
        if (!node) return;
        node->height = std::max(GetHeight(node->left), GetHeight(node->right)) + 1;
    }

    int GetBalance(Node *node) {
        if (!node) return 0;
        return GetHeight(node->right) - GetHeight(node->left);
    }

    size_t GetNumOfElems(Node *node) {
        return node ? node->numOfElems : 0u;
    }

    void FixNumOfElems(Node *node) {
        if (!node) return;
        node->numOfElems = GetNumOfElems(node->left) + GetNumOfElems(node->right) + 1u;
    }

    void DestroyTree(Node *node) {
        if (node) {
            DestroyTree(node->left);
            DestroyTree(node->right);
            delete node;
        }
    }
    Node *root;
    Comparator Compare;
};

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    AvlTree<int, InvertedComparator<int>> buildAvlTree;
    char op;
    int value;
    int numOfOperations = 0;
    in >> numOfOperations;
    while (numOfOperations--) {
        in >> op >> value;
        switch (op) {
            case '1': {
                buildAvlTree.Add(value);
                out << buildAvlTree.GetPosAtSorted(value) << '\n';
                break;
            }
            case '2': {
                buildAvlTree.DeletePos(value);//
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

int main() {
    return run();
}
