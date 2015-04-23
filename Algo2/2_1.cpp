#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_set>
#include <iterator>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <utility>

//  std::make_unique will be available since c++14
//  Implementation was taken from http://herbsutter.com/gotw/_102/
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

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

namespace traverses {

    template<class Visitor, class Graph, class Vertex>
    void BreadthFirstSearch(Vertex origin_vertex, Visitor visitor,
            const Graph& graph) {
        std::queue<Vertex> seen_unprocessed_vertexes;
        std::unordered_set<Vertex> visited_vertexes;
        
        visitor.DiscoverVertex(origin_vertex);
        seen_unprocessed_vertexes.push(origin_vertex);
        visited_vertexes.insert(origin_vertex);
        
        while (!seen_unprocessed_vertexes.empty()) {
            Vertex vertex = seen_unprocessed_vertexes.front();
            seen_unprocessed_vertexes.pop();
            visitor.ExamineVertex(vertex);
            for (const auto &edge : graph.OutgoingEdges(vertex)) {
                visitor.ExamineEdge(edge);
                Vertex target = graph.GetTarget(edge);
                if (visited_vertexes.count(target) == 0) {
                    visitor.DiscoverVertex(target);
                    seen_unprocessed_vertexes.push(graph.GetTarget(edge));
                    visited_vertexes.insert(target);
                }
            }
        }
    }

    template<class Vertex, class Edge>
    class BfsVisitor {
    public:
        virtual void DiscoverVertex(Vertex /*vertex*/) {}
        virtual void ExamineEdge(const Edge& /*edge*/) {}
        virtual void ExamineVertex(Vertex /*vertex*/) {}
        virtual ~BfsVisitor() {}
    };

} // namespace traverses

namespace aho_corasick {

    struct AutomatonNode {
        AutomatonNode():
            suffix_link(nullptr),
            terminal_link(nullptr) {
        }

        std::vector<size_t> matched_string_ids;
        // Stores tree structure of nodes
        std::map<char, AutomatonNode> trie_transitions;
        // Stores pointers to the elements of trie_transitions
        std::map<char, AutomatonNode*> automaton_transitions;
        AutomatonNode* suffix_link;
        AutomatonNode* terminal_link;
    };

    // Returns nullptr if there is no such transition
    AutomatonNode* GetTrieTransition(AutomatonNode* node, char character) {
        auto character_and_node = node->trie_transitions.find(character);
        if (character_and_node == node->trie_transitions.end()) {
            return nullptr;
        }
        return &character_and_node->second;
    }

    // Performs transition in automaton
    AutomatonNode* GetNextNode(AutomatonNode* node, AutomatonNode* root,
            char character) {
        {
            auto character_and_node
                = node->automaton_transitions.find(character);
            if (character_and_node != node->automaton_transitions.end()) {
                return character_and_node->second;
            }
        }
        AutomatonNode* character_and_node = GetTrieTransition(node, character);
        if (character_and_node != nullptr) {
            node->automaton_transitions[character]
                = character_and_node;
            return character_and_node;
        }
        if (node == root) {
            return node;
        }
        return GetNextNode(node->suffix_link, root, character);
    }

    class AutomatonGraph;

    class AutomatonGraph {
    public:
        struct Edge {
            Edge(AutomatonNode* source,
                    AutomatonNode* target,
                    char character):
                source(source),
                target(target),
                character(character) {
            }

            AutomatonNode* source;
            AutomatonNode* target;
            char character;
        };

        // Returns edges corresponding to all trie transitions from vertex
        std::vector<Edge> OutgoingEdges(AutomatonNode* vertex) const {
            std::vector<Edge> outgoing_edges;
            for (auto &character_and_node : vertex->trie_transitions) {
                outgoing_edges.emplace_back(vertex, &character_and_node.second,
                    character_and_node.first);
            }
            return outgoing_edges;
        }

        AutomatonNode* GetTarget(const Edge& edge) const {
            return edge.target;
        }
    };

    class SuffixLinkCalculator:
        public traverses::BfsVisitor<AutomatonNode*, AutomatonGraph::Edge> {
    public:
        explicit SuffixLinkCalculator(AutomatonNode* root):
            root_(root) {}

        void ExamineVertex(AutomatonNode* node) {
            if (node == root_) {
                node->suffix_link = root_;
            }
        }

        void ExamineEdge(const AutomatonGraph::Edge& edge) {
            if (edge.source == root_) {
                edge.target->suffix_link = root_;
            } else {
                edge.target->suffix_link = GetNextNode(edge.source->suffix_link,
                    root_, edge.character);
            }
        }

    private:
        AutomatonNode* root_;
    };

    class TerminalLinkCalculator:
        public traverses::BfsVisitor<AutomatonNode*, AutomatonGraph::Edge> {
    public:
        explicit TerminalLinkCalculator(AutomatonNode* root):
            root_(root) {}

        void DiscoverVertex(AutomatonNode* node) {
            if (node == root_) {
                node->terminal_link = nullptr;
            } else {
                if (!node->suffix_link->matched_string_ids.empty()) {
                    node->terminal_link = node->suffix_link;
                } else {
                    node->terminal_link = node->suffix_link->terminal_link;
                }
            }
        }

    private:
        AutomatonNode* root_;
    };

    class NodeReference {
    public:
        typedef std::vector<size_t>::const_iterator MatchedStringIterator;
        typedef IteratorRange<MatchedStringIterator> MatchedStringIteratorRange;

        NodeReference():
            node_(nullptr),
            root_(nullptr) {
        }
        NodeReference(AutomatonNode* node, AutomatonNode* root):
            node_(node), root_(root) {
        }

        NodeReference Next(char character) const {
            if (node_ == nullptr) {
                return NodeReference();
            }
            return NodeReference(GetNextNode(node_, root_, character), root_);
        }

        NodeReference suffixLink() const {
            if (node_ == nullptr) {
                return NodeReference();
            }
            return NodeReference(node_->suffix_link, root_);
        }

        NodeReference terminalLink() const {
            if (node_ == nullptr) {
                return NodeReference();
            }
            return NodeReference(node_->terminal_link, root_);
        }

        MatchedStringIteratorRange matchedStringIds() const {
            return MatchedStringIteratorRange(node_->matched_string_ids.begin(),
                node_->matched_string_ids.end());
        }

        explicit operator bool() const {
            return node_ != nullptr;
        }

        bool operator==(const NodeReference& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const NodeReference& other) const {
            return node_ != other.node_;
        }

    private:
        AutomatonNode* node_;    
        AutomatonNode* root_;
    };

    using std::rel_ops::operator !=;

    class AutomatonBuilder;

    class Automaton {
    public:
        Automaton() = default;

        NodeReference Root() {
            return NodeReference(&root_, &root_);
        }

        // Calls on_match(string_id) for every string ending at this node,
        // i.e. collects all string ids reachable by terminal links.
        template <class Callback>
        void GenerateMatches(NodeReference node, Callback on_match) {
            while (node) {
                for (size_t matched_string_id : node.matchedStringIds()) {
                    on_match(matched_string_id);
                }
                node = node.terminalLink();
            }
        }

    private:
        AutomatonNode root_;

        Automaton(const Automaton&) = delete;
        Automaton& operator=(const Automaton&) = delete;

        friend class AutomatonBuilder;
    };

    class AutomatonBuilder {
    public:
        void Add(const std::string& string, size_t id) {
            words_.push_back(string);
            ids_.push_back(id);
        }

        std::unique_ptr<Automaton> Build() {
            auto automaton = make_unique<Automaton>();
            BuildTrie(words_, ids_, automaton.get());
            BuildSuffixLinks(automaton.get());
            BuildTerminalLinks(automaton.get());
            return automaton;
        }

    private:
        static void BuildTrie(const std::vector<std::string>& words,
                const std::vector<size_t>& ids,
                Automaton* automaton) {
            for (size_t i = 0; i < words.size(); ++i) {
                AddString(&automaton->root_, ids[i], words[i]);
            }
        }

        static void AddString(AutomatonNode* root, size_t string_id,
                const std::string& string) {
            AutomatonNode* node = root;
            for (char character : string) {
                node = &node->trie_transitions[character];
            }
            node->matched_string_ids.push_back(string_id);
        }

        static void BuildSuffixLinks(Automaton* automaton) {
            SuffixLinkCalculator suffix_link_calculator(&automaton->root_);
            traverses::BreadthFirstSearch(&automaton->root_,
                suffix_link_calculator, AutomatonGraph());
        }

        static void BuildTerminalLinks(Automaton* automaton) {
            TerminalLinkCalculator terminal_link_calculator(&automaton->root_);
            traverses::BreadthFirstSearch(&automaton->root_,
                terminal_link_calculator, AutomatonGraph());
        }

        std::vector<std::string> words_;
        std::vector<size_t> ids_;
    };

}  // namespace aho_corasick

// Consecutive delimiters are not grouped together and are deemed
// to delimit empty strings
template <class Predicate>
std::vector<std::string> Split(const std::string& string,
        Predicate is_delimiter) {
    std::vector<std::string> splitted_string;
    auto begin = string.cbegin();
    auto delimeter = std::find_if(begin, string.cend(), is_delimiter);
    while (delimeter != string.cend()) {
        splitted_string.emplace_back(begin, delimeter);
        begin = std::next(delimeter);
        delimeter = std::find_if(begin, string.cend(), is_delimiter);
    }
    splitted_string.emplace_back(std::string(begin, delimeter));
    return splitted_string;
}

// Wildcard is a character that may be substituted
// for any of all possible characters
class WildcardMatcher {
public:
    WildcardMatcher():
        number_of_words_(0),
        pattern_length_(-1) {
    }

    void Init(const std::string& pattern, char wildcard) {
        pattern_length_ = pattern.size();
        auto patterns_without_wildcard = Split(pattern,
            [&wildcard](char character) {
                return character == wildcard;
            });
        number_of_words_ = patterns_without_wildcard.size();
        aho_corasick::AutomatonBuilder builder;
        size_t pattern_id = 0;
        for (const auto& pattern_without_wildcard: patterns_without_wildcard) {
            pattern_id += pattern_without_wildcard.size();
            builder.Add(pattern_without_wildcard,
                pattern_id > 0 ? pattern_id - 1 : 0);
            ++pattern_id;
        }
        aho_corasick_automaton_ = builder.Build();
        state_ = aho_corasick_automaton_->Root();
    }

    // Resets matcher to start scanning new stream
    void Reset() {
        words_occurrences_by_position_.clear();
    }

    // Scans new character and calls on_match() if
    // suffix of scanned characters matches pattern
    template <class Callback>
    void Scan(char character, Callback on_match) {
        if (!state_) {
            throw std::logic_error("Wildcard matcher wasn't initialized");
        }
        words_occurrences_by_position_.push_front(0);
        state_ = state_.Next(character);
        aho_corasick_automaton_.get()->GenerateMatches(state_,
            [this] (size_t id) mutable {
                if (id < words_occurrences_by_position_.size()) {
                    ++words_occurrences_by_position_[id];
                }
            });
        if (words_occurrences_by_position_.size() == pattern_length_) {
            if (words_occurrences_by_position_.back() == number_of_words_) {
                on_match();
            }
            words_occurrences_by_position_.pop_back();
        }
    }

private:
    std::deque<size_t> words_occurrences_by_position_;
    aho_corasick::NodeReference state_;
    size_t number_of_words_;
    size_t pattern_length_;
    std::unique_ptr<aho_corasick::Automaton> aho_corasick_automaton_;
};

std::string ReadString(std::istream& input_stream) {
    std::string string;
    input_stream >> string;
    return string;
}

// Returns positions of the first character of every match
std::vector<size_t> FindFuzzyMatches(const std::string& patternWithWildcards,
        const std::string& text, char wildcard) {
    size_t patternLength = patternWithWildcards.length();
    WildcardMatcher matcher;
    matcher.Init(patternWithWildcards, wildcard);
    std::vector<size_t> occurrences;
    for (size_t offset = 0; offset < text.size(); ++offset) {
        matcher.Scan(text[offset], [&occurrences, offset, patternLength] () {
                occurrences.push_back(offset + 1 - patternLength);
            });
    }
    return occurrences;
}

template <class Type>
void Print(const std::vector<Type>& sequence) {
    std::cout << sequence.size() << std::endl;
    for (const auto &element : sequence) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

int main() {
    const char wildcard = '?';
    const std::string patternWithWildcards = ReadString(std::cin);
    const std::string text = ReadString(std::cin);
    Print(FindFuzzyMatches(patternWithWildcards, text, wildcard));
    return 0;
}
