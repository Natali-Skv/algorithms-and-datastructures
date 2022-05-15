#include <Huffman.h>
#include <algorithm>
#include <map>
#include <sstream>
#include <stack>
#include <vector>

const size_t ALPH_SIZE = 256u;

// побитовая запись в буфер
class BitWriter {
public:
    BitWriter()
        : bitCount(0u) {
    }

    void WriteBit(unsigned char bit) {
        if (bitCount == buf.size() * 8u)
            buf.push_back(0);
        if (bit)
            buf[bitCount / 8u] |= 1 << (7u - bitCount % 8u);
        bitCount++;
    }

    void WriteByte(unsigned char byte) {
        if (bitCount % 8u == 0)
            buf.push_back(byte);
        else {
            uint8_t offset = bitCount % 8u;
            buf[bitCount / 8] |= byte >> offset;
            buf.push_back(byte << (8u - offset));
        }
        bitCount += 8u;
    }

    [[nodiscard]] const std::vector<unsigned char> &GetBuffer() const {
        return buf;
    }

    [[nodiscard]] size_t GetBitCount() const {
        return bitCount;
    }

private:
    std::vector<myByte> buf;
    size_t bitCount;
};

// побитовое чтение из буфера
class BitReader {
public:
    explicit BitReader(IInputStream &original) : bitCount(0u) {
        myByte value = 0;
        while (original.Read(value)) {
            buffer.push_back(value);
        }
    }

    myByte ReadeBit() {
        if (bitCount == buffer.size() * 8u) {
            return 255u;
        }
        bitCount++;
        return buffer[(bitCount - 1u) / 8u] >> ((8u - bitCount % 8u)) % 8u & 1u;
    }

    myByte ReadByte() {
        if (bitCount == buffer.size() * 8u)
            return 255u;
        myByte offset = bitCount % 8u;
        if (!offset) {
            bitCount += 8u;
            return buffer[(bitCount - 1u) / 8u];
        }
        myByte value = buffer[bitCount / 8u] << offset;
        bitCount += 8u - offset;
        value |= buffer[bitCount / 8u] >> (8u - offset);
        bitCount += offset;
        return value;
    }

private:
    std::vector<unsigned char> buffer;
    size_t bitCount;
};


struct NodeInHuffTree {
    explicit NodeInHuffTree(size_t freq = 0, myByte symbol = 0) : freq(freq), symbol(symbol), left(-1), right(-1) {}
    explicit NodeInHuffTree(size_t freq, int left, int right) : freq(freq), symbol(0u), left(left), right(right) {}

    size_t freq;
    myByte symbol;
    int left; // индекс в векторе левого потомка
    int right; // индекс в векторе правого потомка

    bool operator>(const NodeInHuffTree &rhs) const {
        return (freq > rhs.freq);
    }
    bool operator<(const NodeInHuffTree &rhs) const {
        return (freq < rhs.freq);
    }
};

struct CodeAndSize {
    explicit CodeAndSize() : code(0u), numOfBits(0u) {}
    explicit CodeAndSize(uint64_t code, myByte numOfBits) : code(code), numOfBits(numOfBits) {}
    uint64_t code;
    myByte numOfBits;
    void GoRight() {
        code <<= 1;
        ++code;
        ++numOfBits;
    }
    void GoLeft() {
        code <<= 1;
        ++numOfBits;
    }
    void GoUp() {
        code >>= 1;
        --numOfBits;
    }
};


void AuxBuildCodeTableFromTree(const std::vector<NodeInHuffTree> &nodesOfHuffTree, std::map<unsigned char, CodeAndSize> &codeTable, CodeAndSize &currCodeAndSize, int currIndex) {
    if (currIndex == -1) {
        currCodeAndSize.GoUp();
        return;
    }
    // если это лист -- добавляем в мап запись. (так как оптимальное дерево -- полное, достаточно проверить отсутствие одного потомка)
    if (nodesOfHuffTree[currIndex].left == -1) {
        codeTable.insert(std::make_pair(nodesOfHuffTree[currIndex].symbol, currCodeAndSize));
        currCodeAndSize.GoUp();
        return;
    }
    currCodeAndSize.GoLeft();
    AuxBuildCodeTableFromTree(nodesOfHuffTree, codeTable, currCodeAndSize, nodesOfHuffTree[currIndex].left);
    currCodeAndSize.GoRight();
    AuxBuildCodeTableFromTree(nodesOfHuffTree, codeTable, currCodeAndSize, nodesOfHuffTree[currIndex].right);
    currCodeAndSize.GoUp();
}


std::map<unsigned char, CodeAndSize> BuildCodeTableFromTree(const std::vector<NodeInHuffTree> &nodesOfHuffTree, int rootIndex) {
    std::map<unsigned char, CodeAndSize> codeTable;
    if (nodesOfHuffTree.empty() || (rootIndex > static_cast<int>(nodesOfHuffTree.size()) - 1) || (rootIndex < 0)) {
        return codeTable;
    }
    // если дерево из одного листа
    if (nodesOfHuffTree[rootIndex].left == -1) {
        codeTable.emplace(nodesOfHuffTree[rootIndex].symbol, CodeAndSize(1, 1));
        return codeTable;
    }
    CodeAndSize tmp;
    AuxBuildCodeTableFromTree(nodesOfHuffTree, codeTable, tmp, rootIndex);
    return codeTable;
}

void WriteHuffTree(BitWriter &bw, const std::vector<NodeInHuffTree> &nodesOfHuffTree, int currIndex) {
    if (currIndex == -1) {
        return;
    }
    if (nodesOfHuffTree.empty()) {
        return;
    }
    // если это лист -- в буфер '1' и символ. (так как оптимальное дерево -- полное, достаточно проверить отсутствие одного потомка)
    if (nodesOfHuffTree[currIndex].left == -1) {
        bw.WriteBit(1u);
        bw.WriteByte(nodesOfHuffTree[currIndex].symbol);
        return;
    }
    WriteHuffTree(bw, nodesOfHuffTree, nodesOfHuffTree[currIndex].left);
    WriteHuffTree(bw, nodesOfHuffTree, nodesOfHuffTree[currIndex].right);
    bw.WriteBit(0u);
}

void WriteData(const std::vector<myByte> &buf, const std::map<unsigned char, CodeAndSize> &codeTable, BitWriter &bitWriter) {
    for (myByte i : buf) {
        for (int j = codeTable.at(i).numOfBits - 1; j >= 0; --j) {
            myByte bit = (codeTable.at(i).code >> j) & 1;
            bitWriter.WriteBit(bit);
        }
    }
}


//  [количество символов в алфавите] [количество значащих бит в данных и дерева] [                дерево              ] [данные]
//  <---------1 байт---------------> <--------sizeof(size_t) байт--------------> <---(мощность алфавита)*10-1 бит----->

void Encode(IInputStream &original, IOutputStream &compressed) {
    std::vector<NodeInHuffTree> nodesOfHuffTree(ALPH_SIZE);
    for (size_t i = 0; i < ALPH_SIZE; ++i) {
        nodesOfHuffTree[i].symbol = static_cast<myByte>(i);
    }

    // буфер с исходными данными для кодирования
    std::vector<myByte> dataToCode;
    myByte value = 0u;
    size_t numOfSymbols = 0u;
    while (original.Read(value)) {
        ++numOfSymbols;
        dataToCode.push_back(value);
        ++nodesOfHuffTree[value].freq;
    }

    // удаляем из вектора ноды с нулевым количеством вхождений в файл
    nodesOfHuffTree.erase(std::remove_if(nodesOfHuffTree.begin(), nodesOfHuffTree.end(), [](const NodeInHuffTree &elem) { return !elem.freq; }), nodesOfHuffTree.end());
    if (nodesOfHuffTree.empty()) {
        return;
    }
    // записываем мощность алфавита (макс -- 255)
    compressed.Write(static_cast<myByte>(nodesOfHuffTree.size()));

    // строим дерево хаффмана, корень дерева получится в последнем элементе массива
    // на каждой итерации выбираем 2 минимальных элемента, которые сливаем и кладем новый узел в конец.
    // слитые элементы перемещаем в начало обрабатываемой последовательности [i,nodesOfHuffTree.size())
    for (int i = 0; i < static_cast<int>(nodesOfHuffTree.size()) - 1; i += 2) {
        int min1Ind = i;
        int min2Ind = i + 1;
        if (nodesOfHuffTree[min2Ind] < nodesOfHuffTree[min1Ind]) {
            std::swap(min2Ind, min1Ind);
        }
        for (int j = i + 2; j < static_cast<int>(nodesOfHuffTree.size()); ++j) {
            if (nodesOfHuffTree[min2Ind] > nodesOfHuffTree[j]) {
                if (nodesOfHuffTree[min1Ind] > nodesOfHuffTree[j]) {
                    min2Ind = min1Ind;
                    min1Ind = j;
                } else {
                    min2Ind = j;
                }
            }
        }
        // перемещаем узлы с минимальной freq в начало, чтобы на следующей итерации их не обрабатывать
        std::swap(nodesOfHuffTree[i], nodesOfHuffTree[min1Ind]);
        std::swap(nodesOfHuffTree[i + 1], nodesOfHuffTree[min2Ind]);
        //сливаем два узла с минимальной freq в один
        nodesOfHuffTree.emplace_back(nodesOfHuffTree[i].freq + nodesOfHuffTree[i + 1].freq, i, i + 1);
    }
    nodesOfHuffTree.shrink_to_fit();
    // в последнем элементе  nodesOfHuffTree -- корень дерева Хаффмана
    std::map<unsigned char, CodeAndSize> codeTable = BuildCodeTableFromTree(nodesOfHuffTree, static_cast<int>(nodesOfHuffTree.size()) - 1);

    // получаем в bitWriter.GetBuffer() дерево и данные для записи
    BitWriter bitWriter;
    WriteHuffTree(bitWriter, nodesOfHuffTree, static_cast<int>(nodesOfHuffTree.size()) - 1);
    WriteData(dataToCode, codeTable, bitWriter);

    // побайтово пишем количество значащих бит данных + дерева -- sizeof(size_t) байта
    size_t numOfBits = bitWriter.GetBitCount();
    for (int i = sizeof(numOfBits) - 1; i >= 0; --i) {
        compressed.Write((static_cast<myByte>(numOfBits >> (i * 8) & 255)));
    }

    // дописываем доп 7 бит, чтобы недописанный в буфер бит не пропал
    for (size_t i = 0; i < 7; ++i) {
        bitWriter.WriteBit(0);
    }
    for (unsigned char i : bitWriter.GetBuffer()) {
        compressed.Write(i);
    }
}


struct NodeInHuffTreeDecode {
    explicit NodeInHuffTreeDecode(myByte symbol = 0) : symbol(symbol), left(nullptr), right(nullptr) {}
    NodeInHuffTreeDecode(NodeInHuffTreeDecode *left, NodeInHuffTreeDecode *right) : symbol(0), left(left), right(right) {}
    myByte symbol;
    NodeInHuffTreeDecode *left;
    NodeInHuffTreeDecode *right;
};

//  Начинаем читать само дерево.  Алгоритм:
//  Пока не прочитали столько букв, сколько их в кодируемом алфавите, и пока в стеке более одного узла, читаем бит.
//    Прочитали 1? Читаем еще байт, там символ кодируемого алфавита. Создаем узел дерева Хаффмана с этим символом, кладем узел в стек.
//    Прочитали 0? Достаем из стека два узла, создаем новый, к которому цепляем извлеченные узлы (первый извлеченный -- правый потомок, второй -- левый). Кладем новый узел в стек.

NodeInHuffTreeDecode *RebuildHuffTree(BitReader &bitReader, myByte alphCapacity) {
    std::stack<NodeInHuffTreeDecode *> auxStack;
    while (alphCapacity || (auxStack.size() > 1)) {
        myByte value = bitReader.ReadeBit();

        if (value) {
            myByte symbol = bitReader.ReadByte();
            --alphCapacity;
            auxStack.push(new NodeInHuffTreeDecode(symbol));
        } else {
            NodeInHuffTreeDecode *right = auxStack.top();
            auxStack.pop();
            NodeInHuffTreeDecode *left = auxStack.top();
            auxStack.pop();
            auxStack.push(new NodeInHuffTreeDecode(left, right));
        }
    }
    return auxStack.top();
}

//  Декодируем данные. Читаем побитово, обходим дерево Хаффмана от корня к листьям (прочитали 1 -- идем вправо, 0 -- влево).
//  Дошли до листа -- декодировали очередной символ, пишем его.
//  Не забываем вовремя остановиться, последний байт кодовой последовательности может содержать мусорные биты.
void DecodeData(IOutputStream &original, BitReader &bitReader, NodeInHuffTreeDecode *rootHuffTree, size_t numOfBitsInData) {
    NodeInHuffTreeDecode *iter = rootHuffTree;
    if (!rootHuffTree->left) {
        while (numOfBitsInData--) {
            original.Write(rootHuffTree->symbol);
        }
        return;
    }
    while (numOfBitsInData--) {
        myByte bit = bitReader.ReadeBit();
        if (bit) {
            iter = iter->right;
        } else {
            iter = iter->left;
        }
        // если дошли до листа ( так как дерево полное -- можно не проверять правого потомка)
        if (!iter->left) {
            original.Write(iter->symbol);
            iter = rootHuffTree;
        }
    }
}

void DeleteTree(NodeInHuffTreeDecode *rootHuffTree) {
    if (!rootHuffTree) {
        return;
    }
    DeleteTree(rootHuffTree->left);
    DeleteTree(rootHuffTree->right);
    delete rootHuffTree;
}

//  [количество символов в алфавите] [количество значащих бит в данных] [                дерево              ] [данные]
//  <---------1 байт---------------> <------sizeof(size_t) байт-------> <---(мощность алфавита)*10-1 бит----->

void Decode(IInputStream &compressed, IOutputStream &original) {
    myByte alphCapacity;
    compressed.Read(alphCapacity);
    // количество бит в дереве и в закодированных данных
    size_t numOfBitsInData = 0u;
    myByte value;
    for (size_t i = 0; i < sizeof(numOfBitsInData) && compressed.Read(value); ++i) {
        numOfBitsInData <<= 8u;
        numOfBitsInData |= value;
    }
    if (!numOfBitsInData) {
        return;
    }

    // все считать в буфер битреадер
    BitReader bitReader(compressed);

    NodeInHuffTreeDecode *rootHuffTree = RebuildHuffTree(bitReader, alphCapacity);

    //последний параметр -- количество бит в закодированных данных
    DecodeData(original, bitReader, rootHuffTree, numOfBitsInData - (10u * alphCapacity - 1u));
    DeleteTree(rootHuffTree);
}
