#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iterator>
#include <set>

using std::vector;

template<class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end):
        begin_(begin), end_(end) {}

    Iterator begin() const {
        return begin_;
    }
    Iterator end() const {
        return end_;
    }

private:
    Iterator begin_, end_;
};

namespace graph {
    
    template <class Edge>
    class Graph {
    public:
        typedef IteratorRange<typename vector<Edge>::const_iterator> OutEdgesRange;

        Graph() {}

        explicit Graph(int vertexes_number) {
            adjacency_lists.resize(vertexes_number);
        }

        void AddEdge(const Edge& edge) {
            adjacency_lists[edge.GetSource()].push_back(edge);
        }

        OutEdgesRange OutgoingEdges(int vertex) const {
            return OutEdgesRange(adjacency_lists[vertex].begin(), adjacency_lists[vertex].end());
        }

        size_t GetOutValency(int vertex) const {
            return adjacency_lists[vertex].size();
        }

        size_t VertexesNumber() const {
            return adjacency_lists.size();
        }

    private:
        vector<vector<Edge>> adjacency_lists;
    };

    template <class Graph>
    Graph TransposeGraph(const Graph& graph) {}

    template <class Graph>
    vector<size_t> FindOutVertexesValency(const Graph& graph) {}

    struct Edge {
        Edge() : source(-1), target(-1) {}

        Edge(int source, int target) : source(source), target(target) {}

        int GetSource() const {
            return source;
        }

        int GetTarget() const {
            return target;
        }

        Edge Reverse() const {
            return Edge(target, source);
        }

        bool operator< (const Edge& edge) const {
            if (source != edge.source) {
                return source < edge.source;
            }
            return target < edge.target;
        }

        int source;
        int target;
    };

} // namespace graph

namespace traverses {

    template<class Visitor, class Graph, class VertexIterator>
    void DepthFirstSearchGraph(Visitor& visitor,
                        const Graph& graph,
                        VertexIterator begin,
                        VertexIterator end) {}

    template<class Visitor, class Graph, class Vertex>
    void DepthFirstSearchComponent(Vertex origin_vertex,
                Visitor& visitor,
                const Graph& graph,
                std::unordered_set<Vertex>& visited_vertexes) {}

    template<class Vertex>
    class DfsVisitor {
    public:
        virtual void DiscoverVertex(Vertex /*vertex*/) {}
        virtual void FinishVertex(Vertex /*vertex*/) {}
        virtual void DiscoverComponent() {}
        virtual ~DfsVisitor() {}
    };

} // namespace traverses

namespace components_builder {

    template <class Iterator>
    class OrderBuilder : public traverses::DfsVisitor<int> {
    public:
        explicit OrderBuilder(Iterator position) : position(position) {}

        void FinishVertex(int vertex) {
            *position = vertex;
            ++position;
        }

    private:
        Iterator position;
    };

    template <class Iterator>
    class ComponentsBuilder : public traverses::DfsVisitor<int> {
    public:
        explicit ComponentsBuilder(Iterator position) : position(position),
                                                        current_component(-1) {}

        void DiscoverVertex(int vertex) {
            *(position + vertex) = current_component;
        }

        void DiscoverComponent() {
            ++current_component;
        }

    private:
        Iterator position;
        int current_component;
    };

    // функция возвращает массив размера кол-во вершин; массив описывает принадлежность
    // вершины соответствующей компоненте сильной связности
    template <class Graph>
    vector<int> FindStronglyConnectedComponents(const Graph& graph) {}

    template <class Graph>
    graph::Graph<graph::Edge> CondenceGraph(const vector<int>& components,
                                            const Graph& initial_graph) {}

} // namespace components_builder

struct GameResult {
    enum Outcome {
        WIN = 1,
        LOSS,
        DRAW
    };

    GameResult(int first_candidate,
                int second_candidate,
                Outcome outcome) :
        first_candidate(first_candidate),
        second_candidate(second_candidate),
        outcome(outcome) {
    }

    int first_candidate, second_candidate;
    Outcome outcome;
};

graph::Graph<graph::Edge> MakeRelationshipsGraph(const vector<GameResult>& games_results,
                                                int candidates_number) {}

int FindMaxCompanySize(int candidates_number, const vector<GameResult>& games_results) {}

vector<GameResult> ReadGamesResults() {}

int main() {
    int candidates_number;
    std::cin >> candidates_number;
    vector<GameResult> games_results = ReadGamesResults();
    std::cout << FindMaxCompanySize(candidates_number, games_results) << std::endl;
    return 0;
}
