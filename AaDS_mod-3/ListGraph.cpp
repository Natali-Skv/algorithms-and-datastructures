#include <cassert>
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
        assert(0 <= from && from < adjacencyLists.size());
        assert(0 <= to && to < adjacencyLists.size());
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


int main(int argc, const char *argv[]) {
    ListGraph graph(7);
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

    IGraph *basePtr = &graph;
    mainBFS(*basePtr, [](int vertex) { std::cout << vertex << " "; });
    std::cout << std::endl;

    IGraph &baseLink = graph;
    mainBFS(baseLink, [](int vertex) { std::cout << vertex << " "; });
    std::cout << std::endl;
    return 0;
}
