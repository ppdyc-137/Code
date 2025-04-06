#include <stdexcept>
#include <vector>

struct Edge {
    std::size_t to{};
    int weight{};
};

class Graph {
public:
    virtual void addEdge(std::size_t from, std::size_t to, int weight) = 0;
    virtual void delEdge(std::size_t from, std::size_t to) = 0;
    virtual bool hasEdge(std::size_t from, std::size_t to) = 0;
    virtual int weight(std::size_t from, std::size_t to) = 0;
    virtual ~Graph() = 0;
};

class WeightedGraph : public Graph {
public:
    WeightedGraph(std::size_t n) : graph_(n, std::vector<Edge>(n)) {}

    void addEdge(std::size_t from, std::size_t to, int weight) override { graph_[from].emplace_back(to, weight); }

    void delEdge(std::size_t from, std::size_t to) override {
        auto& table = graph_[from];
        for (auto it = table.begin(); it != table.end(); it++) {
            if (it->to == to) {
                table.erase(it);
                return;
            }
        }
    }

    bool hasEdge(std::size_t from, std::size_t to) override {
        for (auto& edge : graph_[from]) {
            if (edge.to == to) {
                return true;
            }
        }
        return false;
    }

    int weight(std::size_t from, std::size_t to) override {
        for (auto& edge : graph_[from]) {
            if (edge.to == to) {
                return edge.weight;
            }
        }
        throw std::invalid_argument("No such edge");
    }
    const std::vector<Edge>& neighbors(std::size_t from) { return graph_[from]; }

private:
    std::vector<std::vector<Edge>> graph_;
};
