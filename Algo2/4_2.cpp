#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iterator>

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

template <class Edge>
class Graph {
public:
    typedef IteratorRange<typename vector<Edge>::const_iterator> Edges;

    explicit Graph(int vertexes_number) {
        adjacency_lists.resize(vertexes_number);
    }

    void AddEdge(const Edge& edge) {
        adjacency_lists[edge.GetSource()].push_back(edge);
    }

    Edges OutgoingEdges(int vertex) const {
        return Edges(adjacency_lists[vertex].begin(), adjacency_lists[vertex].end());
    }

    size_t GetVertexesNumber() const {
        return adjacency_lists.size();
    }

private:
    vector<vector<Edge>> adjacency_lists;
};

template <class Graph>
Graph FindInverseGraph(const Graph& graph) {}

namespace traverses {

    template<class Visitor, class Graph, class VertexIterator>
    void DepthFirstSearchAllGraph(Visitor& visitor,
                        const Graph& graph,
                        VertexIterator begin,
                        VertexIterator end) {}

    template<class Visitor, class Graph, class Vertex>
    void DFSOneComponent(Vertex origin_vertex,
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

struct GameResult {
    enum Outcome {
        win = 1,
        loss,
        draw
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

struct Relationship {
    Relationship(int first_candidate, int second_candidate):
        first_candidate(first_candidate),
        second_candidate(second_candidate) {
    }

    int GetSource() const {
        return first_candidate;
    }

    int GetTarget() const {
        return second_candidate;
    }

    Relationship Inverte() const {
        return Relationship(second_candidate, first_candidate);
    }

    int first_candidate;
    int second_candidate;
};

namespace components_builder {
    
    class OrderBuilder : public traverses::DfsVisitor<int> {
    public:
        void FinishVertex(int vertex) {
            order.push_back(vertex);
        }

        vector<int> GetOrder() const {
            return order;
        }

    private:
        vector<int> order;
    };

    class ComponentsBuilder : public traverses::DfsVisitor<int> {
    public:
        void DiscoverVertex(int vertex) {
            components.back().push_back(vertex);
        }

        void DiscoverComponent() {
            components.push_back(vector<int>());
        }

        vector<vector<int>> GetComponents() const {
            return components;
        }

    private:
        vector<vector<int>> components;
    };

    template <class Graph>
    vector<vector<int>> FindComponents(const Graph& graph) {}

} // namespace components_builder

Graph<Relationship> MakeRelationshipsGraph(const vector<GameResult>& games_results,
                                            int candidates_number) {}

template <class Graph>
bool CheckPresenceOfIncomingEdges(const vector<int> & component,
                                const vector<bool>& other_view_of_component,
                                const Graph& inverse) {}

template <class Graph>
int FindMinRootComponentSize(const vector<vector<int>>& components,
                            int candidates_number,
                            const Graph& inverse) {}

int FindMaxCompanySize(int candidates_number, const vector<GameResult>& games_results) {}

vector<GameResult> ReadGamesResults() {}

int main() {
    int candidates_number;
    std::cin >> candidates_number;
    vector<GameResult> games_results = ReadGamesResults();
    std::cout << FindMaxCompanySize(candidates_number, games_results) << std::endl;
    return 0;
}
