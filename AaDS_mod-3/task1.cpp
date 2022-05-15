#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

struct IGraph {
    virtual ~IGraph() = default;

    // Добавление ребра от from к vertex.
    virtual void AddEdge(int from, int to) = 0;

    [[nodiscard]] virtual int VerticesCount() const = 0;

    [[nodiscard]] virtual std::vector<int> GetNextVertices(int vertex) const = 0;
    [[nodiscard]] virtual std::vector<int> GetPrevVertices(int vertex) const = 0;

    bool operator==(const IGraph &right) const {
        if (VerticesCount() != right.VerticesCount()) {
            return false;
        }
        std::vector<int> verticesRight;
        std::vector<int> verticesLeft;
        for (int from = 0; from < VerticesCount(); ++from) {
            verticesRight = right.GetNextVertices(from);
            verticesLeft = GetNextVertices(from);
            if (verticesRight.size() != verticesLeft.size()) {
                return false;
            }
            // сортировка нужна, так как разные реализации метода, могут выдавать разный порядок ребер
            std::sort(verticesRight.begin(), verticesRight.end());
            std::sort(verticesLeft.begin(), verticesLeft.end());
            if (verticesRight != verticesLeft) {
                return false;
            }
        }

        for (int to = 0; to < VerticesCount(); ++to) {
            verticesRight = right.GetPrevVertices(to);
            verticesLeft = GetPrevVertices(to);
            if (verticesRight.size() != verticesLeft.size()) {
                return false;
            }
            // сортировка нужна, так как разные реализации метода, могут выдавать разный порядок ребер
            std::sort(verticesRight.begin(), verticesRight.end());
            std::sort(verticesLeft.begin(), verticesLeft.end());
            if (verticesRight != verticesLeft) {
                return false;
            }
        }
        return true;
    }
};

struct ListGraph : public IGraph {
    explicit ListGraph(size_t size)
        : adjacencyLists(size) {
    }

    explicit ListGraph(const IGraph &graph)
        : adjacencyLists(graph.VerticesCount()) {
        for (int i = 0; i < graph.VerticesCount(); i++)
            adjacencyLists[i] = graph.GetNextVertices(i);
    }

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < (int) adjacencyLists.size());
        assert(0 <= to && to < (int) adjacencyLists.size());
        adjacencyLists[from].push_back(to);
    }

    [[nodiscard]] int VerticesCount() const override {
        return (int) adjacencyLists.size();
    }

    [[nodiscard]] std::vector<int> GetNextVertices(int vertex) const override {
        assert(0 <= vertex && vertex < (int) adjacencyLists.size());
        return adjacencyLists[vertex];
    }

    [[nodiscard]] std::vector<int> GetPrevVertices(int vertex) const override {
        assert(0 <= vertex && vertex < (int) adjacencyLists.size());
        std::vector<int> prevVertices;

        for (int from = 0; from < (int) adjacencyLists.size(); from++) {
            for (int to : adjacencyLists[from]) {
                if (to == vertex)
                    prevVertices.push_back(from);
            }
        }
        return prevVertices;
    }

private:
    std::vector<std::vector<int>> adjacencyLists;
};


struct SetGraph : public IGraph {
    explicit SetGraph(size_t size)
        : adjacencySets(size) {
    }

    explicit SetGraph(const IGraph &graph)
        : adjacencySets(graph.VerticesCount()) {
        for (int from = 0; from < graph.VerticesCount(); from++) {
            for (int to : graph.GetNextVertices(from)) {
                adjacencySets[from].insert(to);
            }
        }
    }

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < (int) adjacencySets.size());
        assert(0 <= to && to < (int) adjacencySets.size());
        adjacencySets[from].insert(to);
    }

    [[nodiscard]] int VerticesCount() const override {
        return (int) adjacencySets.size();
    }

    [[nodiscard]] std::vector<int> GetNextVertices(int from) const override {
        assert(0 <= from && from < (int) adjacencySets.size());
        std::vector<int> nextVertices(adjacencySets[from].size());
        std::copy(adjacencySets[from].begin(), adjacencySets[from].end(), nextVertices.begin());
        return nextVertices;
    }

    [[nodiscard]] std::vector<int> GetPrevVertices(int to) const override {
        assert(0 <= to && to < (int) adjacencySets.size());
        std::vector<int> prevVertices;
        for (int from = 0; from < (int) adjacencySets.size(); ++from) {
            if (adjacencySets[from].find(to) != adjacencySets[from].end()) {
                prevVertices.push_back(from);
            }
        }
        return prevVertices;
    }

private:
    std::vector<std::set<int>> adjacencySets;
};


struct MatrixGraph : public IGraph {
    explicit MatrixGraph(size_t size)
        : adjacencyMatrix(size, std::vector<int>(size)) {
    }

    explicit MatrixGraph(const IGraph &graph)
        : adjacencyMatrix(graph.VerticesCount(), std::vector<int>(graph.VerticesCount(), 0)) {
        for (int from = 0; from < graph.VerticesCount(); ++from) {
            for (int to : graph.GetNextVertices(from))
                adjacencyMatrix[from][to] = 1;
        }
    }

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < (int) adjacencyMatrix.size());
        assert(0 <= to && to < (int) adjacencyMatrix.size());
        adjacencyMatrix[from][to] = 1;
    }

    [[nodiscard]] int VerticesCount() const override {
        return (int) adjacencyMatrix.size();
    }

    [[nodiscard]] std::vector<int> GetNextVertices(int from) const override {
        assert(0 <= from && from < (int) adjacencyMatrix.size());
        std::vector<int> nextVertices;
        for (int i = 0; i < (int) adjacencyMatrix.size(); ++i) {
            if (adjacencyMatrix[from][i]) {
                nextVertices.push_back(i);
            }
        }
        return nextVertices;
    }

    [[nodiscard]] std::vector<int> GetPrevVertices(int to) const override {
        assert(0 <= to && to < (int) adjacencyMatrix.size());
        std::vector<int> prevVertices;
        for (int from = 0; from < (int) adjacencyMatrix.size(); ++from) {
            if (adjacencyMatrix[from][to]) {
                prevVertices.push_back(from);
            }
        }
        return prevVertices;
    }

private:
    std::vector<std::vector<int>> adjacencyMatrix;
};



void BFS(const IGraph &graph, int vertex, std::vector<bool> &visited, const std::function<void(int)> &func) {
    std::queue<int> qu;
    qu.push(vertex);

    visited[vertex] = true;

    while (!qu.empty()) {
        int currentVertex = qu.front();
        qu.pop();

        func(currentVertex);

        for (int nextVertex : graph.GetNextVertices(currentVertex)) {
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                qu.push(nextVertex);
            }
        }
    }
}


int main() {
    ListGraph listGraph(7);
    listGraph.AddEdge(0, 1);
    listGraph.AddEdge(0, 5);
    listGraph.AddEdge(1, 2);
    listGraph.AddEdge(1, 3);
    listGraph.AddEdge(1, 5);
    listGraph.AddEdge(1, 6);
    listGraph.AddEdge(3, 2);
    listGraph.AddEdge(3, 4);
    listGraph.AddEdge(3, 6);
    listGraph.AddEdge(5, 4);
    listGraph.AddEdge(5, 6);
    listGraph.AddEdge(6, 4);

    SetGraph setGraph(listGraph);
    MatrixGraph matrixGraph(setGraph);
    ArcGraph arcGraph(matrixGraph);

    if ((listGraph == setGraph) && (setGraph == matrixGraph) && (matrixGraph == arcGraph) && (arcGraph == setGraph)) {
        std::cout << "OK ";
    }

    ArcGraph arcGraph1(10);
    arcGraph1.AddEdge(0, 1);
    arcGraph1.AddEdge(0, 5);
    arcGraph1.AddEdge(1, 2);
    arcGraph1.AddEdge(1, 3);
    arcGraph1.AddEdge(1, 5);
    arcGraph1.AddEdge(1, 6);
    arcGraph1.AddEdge(3, 2);
    arcGraph1.AddEdge(3, 4);
    arcGraph1.AddEdge(3, 6);
    arcGraph1.AddEdge(5, 4);
    arcGraph1.AddEdge(5, 6);
    arcGraph1.AddEdge(6, 4);
    arcGraph1.AddEdge(6, 1);
    arcGraph1.AddEdge(7, 9);
    arcGraph1.AddEdge(8, 5);
    arcGraph1.AddEdge(8, 7);
    arcGraph1.AddEdge(9, 8);
    arcGraph1.AddEdge(9, 1);
    arcGraph1.AddEdge(9, 2);
    arcGraph1.AddEdge(9, 6);

    SetGraph setGraph1(arcGraph1);
    MatrixGraph matrixGraph1(setGraph1);
    ListGraph listGraph1(matrixGraph1);

    if ((listGraph1 == setGraph1) && (setGraph1 == matrixGraph1) && (matrixGraph1 == arcGraph1) && (arcGraph1 == setGraph1)) {
        std::cout << "OK ";
    }

    SetGraph setGraph2(20);
    setGraph2.AddEdge(0, 1);
    setGraph2.AddEdge(2, 1);
    setGraph2.AddEdge(0, 5);
    setGraph2.AddEdge(5, 1);
    setGraph2.AddEdge(10, 2);
    setGraph2.AddEdge(12, 15);
    setGraph2.AddEdge(7, 8);
    setGraph2.AddEdge(4, 6);
    setGraph2.AddEdge(16, 17);
    setGraph2.AddEdge(19, 17);
    setGraph2.AddEdge(5, 2);
    setGraph2.AddEdge(1, 6);
    setGraph2.AddEdge(13, 1);
    setGraph2.AddEdge(0, 13);
    setGraph2.AddEdge(13, 11);
    setGraph2.AddEdge(13, 12);
    setGraph2.AddEdge(15, 12);
    setGraph2.AddEdge(3, 4);
    setGraph2.AddEdge(9, 14);
    setGraph2.AddEdge(14, 17);
    setGraph2.AddEdge(19, 1);
    setGraph2.AddEdge(19, 18);
    setGraph2.AddEdge(8, 12);
    setGraph2.AddEdge(9, 1);

    ArcGraph arcGraph2(setGraph2);
    MatrixGraph matrixGraph2(arcGraph2);
    ListGraph listGraph2(matrixGraph2);

    if ((listGraph2 == setGraph2) && (setGraph2 == matrixGraph2) && (matrixGraph2 == arcGraph2) && (arcGraph2 == setGraph2)) {
        std::cout << "OK";
    }
    return 0;
}
