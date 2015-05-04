#include <iostream>
#include <vector>
#include <algorithm>

typedef std::vector<std::vector<int> > adjacency_lists;

void DFSDirected(int vertex, const adjacency_lists& direct,
        std::vector<int> &order, std::vector<bool>& used) {
    used[vertex] = true;
    for (int target : direct[vertex]) {
        if (!used[target]) {
            DFSDirected(target, direct, order, used);
        }
    }
    order.push_back(vertex);
}

void DFSInversed(int vertex, const adjacency_lists& inverse,
        std::vector<bool> &component, std::vector<bool>& used) {
    used[vertex] = true;
    component[vertex] = true;
    for (int target : inverse[vertex]) {
        if (!used[target]) {
            DFSInversed(target, inverse, component, used);
        }
    }
}

void ExamineComponent(const std::vector<bool>& component,
        const adjacency_lists& inverse, int& minComponentSize) {
    int vertexCnt = static_cast<int>(component.size());
    int componentSize = 0;
    for (int vertex = 0; vertex < vertexCnt; ++vertex) {
        if (component[vertex]) {
            for (int target : inverse[vertex]) {
                if (!component[target]) {
                    return;
                }
            }
            ++componentSize;
        }
    }
    minComponentSize = std::min(minComponentSize, componentSize);
}

void ReadGraph(adjacency_lists& direct, adjacency_lists& inverse) {
    int vertexCount, edgeCount;
    std::cin >> vertexCount >> edgeCount;
    direct.resize(vertexCount);
    inverse.resize(vertexCount);
    for (int i = 0; i < edgeCount; ++i) {
        int source, target, weight;
        std::cin >> source >> target >> weight;
        if (weight == 1) {
            direct[source - 1].push_back(target - 1);
            inverse[target - 1].push_back(source - 1);
        } else if (weight == 2) {
            direct[target - 1].push_back(source - 1);
            inverse[source - 1].push_back(target - 1);
        }
    }
}

int FindMaxCompanySize(const adjacency_lists& direct,
        const adjacency_lists& inverse) {
    int vertexCount = static_cast<int>(direct.size());
    std::vector<bool> used(vertexCount, false);
    std::vector<int> order;
    for (int i = 0; i< vertexCount; ++i) {
        if (!used[i]) {
            DFSDirected(i, direct, order, used);
        }
    }
    used.assign(vertexCount, false);
    int minComponentSize = vertexCount;
    for (int i = 0; i < vertexCount; ++i) {
        int vertex = order[vertexCount - 1 - i];
        if (!used[vertex]) {
            std::vector<bool> component(vertexCount, false);
            DFSInversed(vertex, inverse, component, used);
            ExamineComponent(component, inverse, minComponentSize);
        }
    }
    return vertexCount - (minComponentSize - 1);
}

int main() {
    adjacency_lists direct, inverse;
    ReadGraph(direct, inverse);
    std::cout << FindMaxCompanySize(direct, inverse) << "\n";
}
