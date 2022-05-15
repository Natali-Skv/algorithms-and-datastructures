#include <deque>
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
/*
 Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
 С помощью алгоритма Прима.
 */

const size_t MAX_EDGE = 100001u;

struct ListGraph {

    explicit ListGraph(size_t size)
        : adjacencyLists(size) {
    }

    void AddEdge(uint32_t from, uint32_t to, size_t weight) {
        adjacencyLists[from].emplace_back(weight, to);
        adjacencyLists[to].emplace_back(weight, from);
    }

    [[nodiscard]] size_t VerticesCount() const {
        return adjacencyLists.size();
    }

    [[nodiscard]] std::vector<std::pair<size_t, uint32_t>> GetNextVertices(size_t vertex) const {
        return adjacencyLists[vertex];
    }

    [[nodiscard]] size_t GetWeightMST() const {
        std::vector<bool> addedToST(VerticesCount(), false);
        std::vector<size_t> minEdgeToST(VerticesCount(), MAX_EDGE);
        minEdgeToST[0] = 0u;
        std::set<std::pair<size_t, uint32_t>> set;
        set.emplace(0u, 0u);
        while (!set.empty()) {
            auto currVertex = set.extract(set.begin()).value();
            addedToST[currVertex.second] = true;
            for (auto nextVertex : GetNextVertices(currVertex.second)) {
                if ((!addedToST[nextVertex.second]) && (minEdgeToST[nextVertex.second] > nextVertex.first)) {
                    minEdgeToST[nextVertex.second] = nextVertex.first;
                    set.erase(std::make_pair(minEdgeToST[nextVertex.second], nextVertex.second));
                    set.emplace(nextVertex);
                }
            }
        }
        size_t weightMST = 0u;
        for (auto i : minEdgeToST) {
            if (i != MAX_EDGE) {
                weightMST += i;
            }
        }
        return weightMST;
    }

private:
    std::vector<std::vector<std::pair<size_t, uint32_t>>> adjacencyLists;
};

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    size_t verticesCount;
    in >> verticesCount;
    size_t arcCount;
    in >> arcCount;
    size_t v1, v2, weight;
    ListGraph graph(verticesCount);
    while (arcCount--) {
        in >> v1 >> v2 >> weight;
        // отнимаем 1, так как в реализации графа номерация вершин идет с 0
        graph.AddEdge(v1 - 1u, v2 - 1u, weight);
    }
    out << graph.GetWeightMST();
    return 0;
}

int main() {
    return run();
}