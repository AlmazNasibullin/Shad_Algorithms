#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

namespace traverses {

    template<class Visitor, class Graph, class Vertex>
    void DepthFirstSearch(Vertex origin_vertex,
                        Visitor& visitor,
                        const Graph& graph,
                        const std::vector<Vertex>& traversal) {}

    template<class Visitor, class Graph, class Vertex>
    void DFSVisit(Vertex origin_vertex,
                Visitor& visitor,
                const Graph& graph,
                std::unordered_set<Vertex>& visited_vertexes) {}

    template<class Vertex, class Edge>
    class DfsVisitor {
    public:
        virtual void DiscoverVertex(Vertex /*vertex*/) {}
        virtual void ExamineEdge(const Edge& /*edge*/) {}
        virtual void FinishVertex(Vertex /*vertex*/) {}
        virtual void DiscoverComponent() {}
        virtual ~DfsVisitor() {}
    };

} // namespace traverses

struct GameResult {
    int first_candidate, second_candidate;
    int outcome;

    GameResult(int first_candidate,
                int second_candidate,
                int outcome) :
        first_candidate(first_candidate),
        second_candidate(second_candidate),
        outcome(outcome) {
    }
};

class CompanyGraph {
public:
    struct Edge {
        Edge(int first_candidate, int second_candidate):
            first_candidate(first_candidate),
            second_candidate(second_candidate) {
        }

        int first_candidate;
        int second_candidate;
    };

    std::vector<Edge> OutgoingEdges(int candidate) const {}

    int GetTarget(const Edge& edge) const {
        return edge.second_candidate;
    }

public:
    explicit CompanyGraph(int candidates_number) {
        relationships.resize(candidates_number);
    }

    void AddRelationship(int first_candidate, int second_candidate) {
        relationships[first_candidate].push_back(second_candidate);
    }

public:
    std::vector<std::vector<int> > relationships;
};


namespace components_builder {
    
    class OrderBuilder : public traverses::DfsVisitor<int, CompanyGraph::Edge> {
    public:
        void FinishVertex(int vertex) {
            order.push_back(vertex);
        }

        std::vector<int> GetOrder() const {
            return order;
        }

    private:
        std::vector<int> order;
    };

    class ComponentsBuilder : public traverses::DfsVisitor<int, CompanyGraph::Edge> {
    public:
        void DiscoverVertex(int vertex) {
            component.push_back(vertex);
        }

        void DiscoverComponent() {
            components.push_back(component);
            component.clear();
        }

        std::vector<std::vector<int> > GetComponents() const {
            return components;
        }

    private:
        std::vector<int> component;
        std::vector<std::vector<int> > components;
    };

} // namespace components_builder

class MaxCompanyBuilder {
public:
    explicit MaxCompanyBuilder(int candidates_number):
            candidates_number(candidates_number),
            directed(CompanyGraph(candidates_number)),
            inverse(CompanyGraph(candidates_number)) { 
    }

    void Init(const std::vector<GameResult>& games_results) {}

    int FindMaxCompanySize() {
        std::vector<std::vector<int> > components = FindComponents();
        // root component = no incoming edges
        int min_root_component_size = FindMinRootComponentSize(components);
        return candidates_number - (min_root_component_size - 1);
    }

private:
    std::vector<std::vector<int> > FindComponents() {}

    // root component = no incoming edges
    int FindMinRootComponentSize(const std::vector<std::vector<int> >&
            components) {}

    bool CheckPresenceOfIncomingEdges(const std::vector<int> & component,
                                    const std::vector<bool>&
                                    other_view_of_component) {}

private:
    CompanyGraph directed, inverse;
    int candidates_number;
};

std::vector<GameResult> ReadGamesResults(int candidates_number,
                                        int games_number) {}

int main() {
    int candidates_number, games_number;
    std::cin >> candidates_number >> games_number;
    std::vector<GameResult> games_results = ReadGamesResults(candidates_number,
                                                            games_number);
    MaxCompanyBuilder mcb(candidates_number);
    mcb.Init(games_results);
    std::cout << mcb.FindMaxCompanySize() << std::endl;
    return 0;
}
