#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

struct IGraph {
    virtual ~IGraph() = default;

    // Добавление ребра от from к vertex.
    virtual void AddEdge(int from, int to) = 0;

    [[nodiscard]] virtual int VerticesCount() const = 0;

    [[nodiscard]] virtual std::vector<int> GetNextVertices(int vertex) const = 0;
    [[nodiscard]] virtual std::vector<int> GetPrevVertices(int vertex) const = 0;
};

struct ArcGraph : public IGraph {
    explicit ArcGraph(size_t size)
        : verticesCount(size) {
    }

    explicit ArcGraph(const IGraph &graph)
        : verticesCount(graph.VerticesCount()) {
        for (int from = 0; from < verticesCount; ++from) {
            for (int to : graph.GetNextVertices(from)) {
                arcVec.emplace_back(from, to);
            }
        }
    }

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < verticesCount);
        assert(0 <= to && to < verticesCount);
        arcVec.emplace_back(from,to);
    }

    [[nodiscard]] int VerticesCount() const override {
        return verticesCount;
    }

    [[nodiscard]] std::vector<int> GetNextVertices(int from) const override {
        assert(0 <= from && from < (int) verticesCount);
        std::vector<int> nextVertices;
        for(auto &arc:arcVec) {
            if (arc.from == from) {
                nextVertices.push_back(arc.to);
            }
        }
        return nextVertices;
    }

    [[nodiscard]] std::vector<int> GetPrevVertices(int to) const override {
        assert(0 <= to && to < (int) verticesCount);
        std::vector<int> prevVertices;
        for(auto &arc:arcVec) {
            if (arc.to == to) {
                prevVertices.push_back(arc.from);
            }
        }
        return prevVertices;
    }

private:
    struct Arc {
        Arc(int from, int to) : from(from), to(to) {}
        int from;
        int to;
    };
    std::vector<Arc> arcVec;
    int verticesCount;
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

void mainBFS(const IGraph &graph, const std::function<void(int)> &func) {
    std::vector<bool> visited(graph.VerticesCount(), false);
    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i])
            BFS(graph, i, visited, func);
    }
}

void DFS(const IGraph &graph, int vertex, std::vector<bool> &visited, std::function<void(int)> &func) {
    visited[vertex] = true;
    func(vertex);

    for (int nextVertex : graph.GetNextVertices(vertex)) {
        if (!visited[nextVertex])
            DFS(graph, nextVertex, visited, func);
    }
}

void mainDFS(const IGraph &graph, std::function<void(int)> func) {
    std::vector<bool> visited(graph.VerticesCount(), false);
    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i])
            DFS(graph, i, visited, func);
    }
}

void topologicalSortInternal(const IGraph &graph, int vertex, std::vector<bool> &visited, std::deque<int> &sorted) {
    visited[vertex] = true;

    for (int nextVertex : graph.GetNextVertices(vertex)) {
        if (!visited[nextVertex])
            topologicalSortInternal(graph, nextVertex, visited, sorted);
    }

    sorted.push_front(vertex);
}

std::deque<int> topologicalSort(const IGraph &graph) {
    std::deque<int> sorted;
    std::vector<bool> visited(graph.VerticesCount(), false);

    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i])
            topologicalSortInternal(graph, i, visited, sorted);
    }

    return sorted;
}

int main(int argc, const char *argv[]) {
    ArcGraph graph(7);
    graph.AddEdge(0, 1);
    graph.AddEdge(0, 5);
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(1, 5);
    graph.AddEdge(1, 6);
    graph.AddEdge(3, 2);
    graph.AddEdge(3, 4);
    graph.AddEdge(3, 6);
    graph.AddEdge(5, 4);
    graph.AddEdge(5, 6);
    graph.AddEdge(6, 4);

    mainBFS(graph, [](int vertex) { std::cout << vertex << " "; });
    std::cout << std::endl;
    mainDFS(graph, [](int vertex) { std::cout << vertex << " "; });
    std::cout << std::endl;

    for (int vertex : topologicalSort(graph)) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    // MatrixGraph matrixGraph(graph);
    // std::cout << matrixGraph.GetVerticesCount() << std::endl;
    // mainBFS(matrixGraph, [](int vertex){ std::cout << vertex << " ";});
    // std::cout << std::endl;
    // mainDFS(matrixGraph, [](int vertex){ std::cout << vertex << " ";});
    // std::cout << std::endl;

    // ArcGraph arcGraph(matrixGraph);
    // ...

    // SetGraph setGraph(arcGraph);
    // ...
    return 0;
}
