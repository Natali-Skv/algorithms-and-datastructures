#include <cassert>
#include <climits>
#include <deque>
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

/*
Количество различных путей
Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей между какими-то вершинами.
Найдите количество различных кратчайших путей между заданными вершинами.
Требования: сложность O(V+E).
 */


struct ListGraph {
    explicit ListGraph(size_t size)
        : adjacencyLists(size) {
    }

    void AddEdge(int from, int to) {
        assert(0 <= from && from < (int) adjacencyLists.size());
        assert(0 <= to && to < (int) adjacencyLists.size());
        adjacencyLists[from].push_back(to);
    }

    [[nodiscard]] int VerticesCount() const {
        return (int) adjacencyLists.size();
    }

    [[nodiscard]] std::vector<int> GetNextVertices(int vertex) const {
        assert(0 <= vertex && vertex < (int) adjacencyLists.size());
        return adjacencyLists[vertex];
    }

    [[nodiscard]] int numOfShortestPath(int from, int to) const {
        // расстояние от from до radius[i]
        std::vector<int> radius(VerticesCount(), INT_MAX);
        std::vector<int> countsRadius(VerticesCount(), 0);
        std::queue<int> queue;
        queue.push(from);
        radius[from] = 0;
        countsRadius[from] = 1;
        while (!queue.empty()) {
            int currVertex = queue.front();
            queue.pop();
            for (int nextVertex : GetNextVertices(currVertex)) {
                if (radius[nextVertex] == radius[currVertex] + 1) {
                    countsRadius[nextVertex] += countsRadius[currVertex];
                } else if (radius[nextVertex] > radius[currVertex] + 1) {
                    radius[nextVertex] = radius[currVertex] + 1;
                    countsRadius[nextVertex] = countsRadius[currVertex];
                    queue.push(nextVertex);
                }
            }
        }
        return countsRadius[to];
    }

private:
    std::vector<std::vector<int>> adjacencyLists;
};

int run(std::istream &in = std::cin, std::ostream &out = std::cout) {
    int verticesCount;
    in >> verticesCount;
    int arcCount;
    in >> arcCount;
    int v1, v2;
    ListGraph graph(verticesCount);
    while (arcCount--) {
        in >> v1 >> v2;
        graph.AddEdge(v1, v2);
        if (v1 != v2) {
            graph.AddEdge(v2, v1);
        }
    }
    in >> v1 >> v2;
    out << graph.numOfShortestPath(v1, v2);
    return 0;
}

int main() {
    return run();
}