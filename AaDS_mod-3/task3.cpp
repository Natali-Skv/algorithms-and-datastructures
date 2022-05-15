#include <cassert>
#include <climits>
#include <deque>
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

// Требуется отыскать самый выгодный маршрут между городами.
// Требования: время работы O((N+M)logN), где N-количество городов, M-известных дорог между ними.
// Формат входных данных.
//      Первая строка содержит число N – количество городов.
//      Вторая строка содержит число M - количество дорог.
//      Каждая следующая строка содержит описание дороги (откуда, куда, время в пути).
//      Последняя строка содержит маршрут (откуда и куда нужно доехать).
// Формат выходных данных.
//      Вывести длину самого выгодного маршрута.

struct ListGraph {

    explicit ListGraph(size_t size)
        : adjacencyLists(size) {
    }

    void AddEdge(int from, int to, int weight) {
        assert(0 <= from && from < (int) adjacencyLists.size());
        assert(0 <= to && to < (int) adjacencyLists.size());
        assert(0 <= weight);
        adjacencyLists[from].emplace_back(weight, to);
        adjacencyLists[to].emplace_back(weight, from);
    }

    [[nodiscard]] int VerticesCount() const {
        return (int) adjacencyLists.size();
    }

    [[nodiscard]] std::vector<std::pair<int, int>> GetNextVertices(int vertex) const {
        assert(0 <= vertex && vertex < (int) adjacencyLists.size());
        return adjacencyLists[vertex];
    }

    [[nodiscard]] int GetWeightMST(int from, int to) const {
        std::vector<int> radius(VerticesCount(), INT_MAX);
        radius[from] = 0;
        std::set<std::pair<int, int>> set;
        set.emplace(0, from);
        while (!set.empty()) {
            auto currVertex = set.extract(set.begin()).value();
            for (auto nextVertex : GetNextVertices(currVertex.second)) {
                if (radius[currVertex.second] + nextVertex.first < radius[nextVertex.second]) {
                    set.erase(std::pair(radius[nextVertex.second], nextVertex.second));
                    radius[nextVertex.second] = radius[currVertex.second] + nextVertex.first;
                    set.emplace(radius[nextVertex.second], nextVertex.second);
                }
            }
        }
        return radius[to];
    }

private:
    std::vector<std::vector<std::pair<int, int>>> adjacencyLists;
};

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    int verticesCount;
    in >> verticesCount;
    int arcCount;
    in >> arcCount;
    int v1, v2, weight;
    ListGraph graph(verticesCount);
    while (arcCount--) {
        in >> v1 >> v2 >> weight;
        graph.AddEdge(v1, v2, weight);
    }
    in >> v1 >> v2;
    out << graph.GetWeightMST(v1, v2);
    return 0;
}

int main() {
    return run();
}
