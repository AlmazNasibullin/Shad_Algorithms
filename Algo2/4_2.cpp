#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iterator>
#include <set>
#include <boost/range/counting_range.hpp>

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
        typedef Edge edge_type;

        Graph() {}

        explicit Graph(int vertexes_number) {
            adjacency_lists.resize(vertexes_number);
        }

        void AddEdge(const edge_type& edge) {
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
        vector<vector<edge_type>> adjacency_lists;
    };

    template <class Graph>
    Graph TransposeGraph(const Graph& graph) {
        int vertexes_number = static_cast<int>(graph.VertexesNumber());
        Graph transposedGraph(vertexes_number);
        for (int vertex = 0; vertex < vertexes_number; ++vertex) {
            for (const auto& edge : graph.OutgoingEdges(vertex)) {
                transposedGraph.AddEdge(edge.Reverse());
            }
        }
        return transposedGraph;
    }

    template <class Graph>
    vector<size_t> FindOutVertexesValency(const Graph& graph) {
        int vertexes_number = static_cast<int>(graph.VertexesNumber());
        vector<size_t> out_vertexes_valency(vertexes_number);
        for (int vertex = 0; vertex < vertexes_number; ++vertex) {
            out_vertexes_valency[vertex] = graph.GetOutValency(vertex);
        }
        return out_vertexes_valency;
    }

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
                        VertexIterator end) {

        std::unordered_set<typename std::iterator_traits<VertexIterator>::value_type>
            visited_vertexes;

        for (; begin != end; ++begin) {
            if (visited_vertexes.count(*begin) == 0) {
                visitor.DiscoverComponent();
                DepthFirstSearchComponent(*begin, visitor, graph, visited_vertexes);
            }
        }
    }

    template<class Visitor, class Graph, class Vertex>
    void DepthFirstSearchComponent(Vertex origin_vertex,
                Visitor& visitor,
                const Graph& graph,
                std::unordered_set<Vertex>& visited_vertexes) {
        visitor.DiscoverVertex(origin_vertex);
        visited_vertexes.insert(origin_vertex);
        
        for (const auto &edge : graph.OutgoingEdges(origin_vertex)) {
            Vertex target = edge.GetTarget();
            if (visited_vertexes.count(target) == 0) {
                DepthFirstSearchComponent(target, visitor, graph, visited_vertexes);
            }
        }
        
        visitor.FinishVertex(origin_vertex);
    }

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
    vector<int> FindStronglyConnectedComponents(const Graph& graph) {
        vector<int> backward_order(graph.VertexesNumber());
        OrderBuilder<vector<int>::iterator> order_builder(backward_order.begin());
        { // прямой обход 
            traverses::DepthFirstSearchGraph(order_builder,
                                            graph,
                                            boost::counting_iterator<int>(0),
                                            boost::counting_iterator<int>(graph.VertexesNumber()));
        }
        { // обратный обход
            std::reverse(backward_order.begin(), backward_order.end());
            vector<int> components(graph.VertexesNumber());
            ComponentsBuilder<vector<int>::iterator> components_builder(components.begin());
            traverses::DepthFirstSearchGraph(components_builder,
                                            graph::TransposeGraph(graph),
                                            backward_order.begin(),
                                            backward_order.end());
            return components;
        }
    }

    template <class Graph>
    graph::Graph<graph::Edge> CondenceGraph(const vector<int>& components,
                                            const Graph& initial_graph) {
        int components_number = 1;
        for (int component_number : components) {
            components_number = std::max(components_number, component_number + 1);
        }
        int vertexes_number = static_cast<int>(initial_graph.VertexesNumber());
        std::set<graph::Edge> edges_in_condence_graph;

        for (int vertex = 0; vertex < vertexes_number; ++vertex) {
            for (const auto& edge : initial_graph.OutgoingEdges(vertex)) {
                if (components[edge.GetSource()] != components[edge.GetTarget()]) {
                   edges_in_condence_graph.insert(graph::Edge(components[edge.GetSource()],
                                                                components[edge.GetTarget()]));
               }
            }
        }

        graph::Graph<graph::Edge> condece_graph(components_number);
        for (auto& edge : edges_in_condence_graph) {
            condece_graph.AddEdge(edge);    
        }
        return condece_graph;
    }

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
                                                int candidates_number) {
    graph::Graph<graph::Edge> relationshipsGraph(candidates_number);
    for (const auto& game_result : games_results) {
        GameResult::Outcome outcome = game_result.outcome;
        if (outcome == GameResult::Outcome::WIN) {
            relationshipsGraph.AddEdge(graph::Edge(game_result.first_candidate,
                                                    game_result.second_candidate));
        } else if (outcome == GameResult::Outcome::LOSS) {
            relationshipsGraph.AddEdge(graph::Edge(game_result.second_candidate,
                                                    game_result.first_candidate));
        }
    }
    return relationshipsGraph;
}

int FindMaxCompanySize(int candidates_number, const vector<GameResult>& games_results) {
    graph::Graph<graph::Edge> relationshipsGraph = MakeRelationshipsGraph(games_results,
                                                                            candidates_number);
    vector<int> components;
    components = components_builder::FindStronglyConnectedComponents(relationshipsGraph);
    graph::Graph<graph::Edge> condece_graph;
    condece_graph = components_builder::CondenceGraph(components, relationshipsGraph);

    vector<size_t> vertexes_incoming_valency_of_condence_graph = graph::FindOutVertexesValency(
        graph::TransposeGraph(condece_graph));

    vector<int> components_sizes(condece_graph.VertexesNumber());
    for (int component_number : components) {
        ++components_sizes[component_number];
    }
    
    int min_root_component_size = candidates_number;
    for (size_t component = 0; component < condece_graph.VertexesNumber(); ++component) {
        if (vertexes_incoming_valency_of_condence_graph[component] == 0) {
            min_root_component_size = std::min(min_root_component_size,
                                                components_sizes[component]);
        }
    }
    return candidates_number - min_root_component_size + 1;
}

vector<GameResult> ReadGamesResults() {
    int games_number;
    std::cin >> games_number;
    
    vector<GameResult> games_results;
    for (int i = 0; i < games_number; ++i) {
        int first_candidate;
        int second_candidate;
        int outcome;
        std::cin >> first_candidate >> second_candidate >> outcome;
        games_results.emplace_back(first_candidate - 1,
                                    second_candidate - 1,
                                    static_cast<GameResult::Outcome>(outcome));
    }
    return games_results;
}

int main() {
    int candidates_number;
    std::cin >> candidates_number;
    vector<GameResult> games_results = ReadGamesResults();
    std::cout << FindMaxCompanySize(candidates_number, games_results) << std::endl;
    return 0;
}
