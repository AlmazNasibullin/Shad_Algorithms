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
    void BreadthFirstSearch(Vertex origin_vertex, const Graph& graph, Visitor visitor);

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
        AutomatonNode* parent;
        int number;
        char character;
    };

    // Returns nullptr if there is no such transition
    AutomatonNode* GetTrieTransition(AutomatonNode* node, char character) {
        std::map<char, AutomatonNode>::iterator pos
            = node->trie_transitions.find(character);
        if (pos == node->trie_transitions.end()) {
            return nullptr;
        }
        return &pos->second;
    }

    // Performs transition in automaton
    AutomatonNode* GetNextNode(AutomatonNode* node, AutomatonNode* root,
            char character) {
        while (node != root) {
            if (GetTrieTransition(node, character) != nullptr) {
                return GetTrieTransition(node, character);
            }
            node = node->suffix_link;
        }
        if (GetTrieTransition(node, character) != nullptr) {
            return GetTrieTransition(node, character);
        }
        return node;
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
        static std::vector<Edge> OutgoingEdges(AutomatonNode* vertex) {
            std::vector<Edge> outgoingEdges;
            for (auto &transition : vertex->automaton_transitions) {
                outgoingEdges.push_back(Edge(vertex, transition.second,
                    transition.first));
            }
            return outgoingEdges;
        }
    };

    AutomatonNode* GetTarget(const AutomatonGraph::Edge& edge) {
        return edge.target;
    }

    class SuffixLinkCalculator:
        public traverses::BfsVisitor<AutomatonNode*, AutomatonGraph::Edge> {
    public:
        explicit SuffixLinkCalculator(AutomatonNode* root):
            root_(root) {}

        void ExamineVertex(AutomatonNode* node) {
            if (node == root_ || node->parent == root_) {
                node->suffix_link = root_;
            } else {
                node->suffix_link = GetNextNode(node->parent->suffix_link,
                    root_, node->character);
            }
        }

        void ExamineEdge(const AutomatonGraph::Edge& edge) {}

    private:
        AutomatonNode* root_;
    };

    class TerminalLinkCalculator:
        public traverses::BfsVisitor<AutomatonNode*, AutomatonGraph::Edge> {
    public:
        explicit TerminalLinkCalculator(AutomatonNode* root):
            root_(root) {}

        void DiscoverVertex(AutomatonNode* node) {
            if (node == root_ || node->parent == root_) {
                node->terminal_link = root_;
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

        int getNumber() const {
            return node_->number;
        }

        explicit operator bool() const {
            return node_ != nullptr;
        }

        bool operator==(NodeReference other) const {
            return node_ == other.node_;
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
            while (node != Root()) {
                NodeReference::MatchedStringIteratorRange range
                    = node.matchedStringIds();
                NodeReference::MatchedStringIterator iterator;
                for (iterator = range.begin(); iterator != range.end();
                        ++iterator) {
                    on_match(*iterator);
                }
                node = node.terminalLink();
            }
        }

        void print() {
            std::queue<AutomatonNode*> queue;
            queue.push(&root_);
            while (!queue.empty()) {
                AutomatonNode* cur = queue.front();
                queue.pop();
                std::map<char, AutomatonNode*> transitions
                    = cur->automaton_transitions;
                std::vector<size_t> ids = cur->matched_string_ids;
                std::cout << "ids: ";
                for (auto id : ids) {
                    std::cout << id << " ";
                }
                std::cout << "tran: ";
                for (auto& el : transitions) {
                    std::cout << el.first << " ";
                    queue.push(el.second);
                }
                std::cout << "num: ";
                std::cout << cur->number << " -- "
                    << cur->suffix_link->number << " -- "
                    << cur->terminal_link->number << std::endl;
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
            // automaton.get()->print();
            return automaton;
        }

    private:
        static void BuildTrie(const std::vector<std::string>& words,
                const std::vector<size_t>& ids,
                Automaton* automaton) {
            int number = 1;
            for (size_t i = 0; i < words.size(); ++i) {
                AddString(&automaton->root_, ids[i], words[i], number);
            }
        }

        static void AddString(AutomatonNode* root, size_t string_id,
                const std::string& string, int& number) {
            AutomatonNode* cur = root;
            for (char character : string) {
                AutomatonNode* nextNode = GetTrieTransition(cur, character);
                if (nextNode == nullptr) {
                    cur->trie_transitions.insert(std::pair<char, AutomatonNode>
                        (character, AutomatonNode()));
                    cur->automaton_transitions[character]
                        = &(cur->trie_transitions.find(character)->second);
                    nextNode = GetTrieTransition(cur, character);
                    nextNode->parent = cur;
                    nextNode->character = character;
                    nextNode->number = number++;
                }
                cur = nextNode;
            }
            cur->matched_string_ids.push_back(string_id);
        }

        static void BuildSuffixLinks(Automaton* automaton) {
            SuffixLinkCalculator suffixLinkCalculator(&automaton->root_);
            std::queue<AutomatonNode*> queue;
            queue.push(&automaton->root_);
            while (!queue.empty()) {
                AutomatonNode* cur = queue.front();
                queue.pop();
                suffixLinkCalculator.ExamineVertex(cur);
                std::vector<AutomatonGraph::Edge> outgoingEdges
                    = AutomatonGraph::OutgoingEdges(cur);
                for (auto &edge : outgoingEdges) {
                    queue.push(GetTarget(edge));
                }
            }
        }

        static void BuildTerminalLinks(Automaton* automaton) {
            TerminalLinkCalculator terminalLinkCalculator(&automaton->root_);
            std::queue<AutomatonNode*> queue;
            queue.push(&automaton->root_);
            while (!queue.empty()) {
                AutomatonNode* cur = queue.front();
                queue.pop();
                terminalLinkCalculator.DiscoverVertex(cur);
                std::vector<AutomatonGraph::Edge> outgoingEdges
                    = AutomatonGraph::OutgoingEdges(cur);
                for (auto &edge : outgoingEdges) {
                    queue.push(GetTarget(edge));
                }
            }
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
    std::vector<std::string> result;
    std::string currentSubstr = "";
    for (char character : string) {
        if (is_delimiter(character)) {
            result.push_back(currentSubstr);
            currentSubstr = "";
        } else {
            currentSubstr += character;
        }
    }
    result.push_back(currentSubstr);
    return result;
}

// Wildcard is a character that may be substituted
// for any of all possible characters
class WildcardMatcher {
public:
    WildcardMatcher():
        number_of_words_(0),
        pattern_length_(0) {
    }

    void Init(const std::string& pattern, char wildcard) {
        pattern_length_ = pattern.size();
        words_occurrences_by_position_.resize(pattern_length_, 0);
        std::vector<std::string> patternsWithoutWildcard = Split(pattern,
            [&wildcard](char character) {
                return character == wildcard;
            });
        aho_corasick::AutomatonBuilder builder;
        size_t patternId = 0;
        for (auto& patternWithoutWildcard: patternsWithoutWildcard) {
            patternId += patternWithoutWildcard.size();
            if (!patternWithoutWildcard.empty()) {
                builder.Add(patternWithoutWildcard, patternId - 1);
                ++number_of_words_;
            }
            ++patternId;
        }
        aho_corasick_automaton_ = builder.Build();
        state_ = aho_corasick_automaton_->Root();
    }

    // Resets matcher to start scanning new stream
    void Reset() {
        words_occurrences_by_position_.clear();
        words_occurrences_by_position_.resize(pattern_length_, 0);
    }

    // Scans new character and calls on_match() if
    // suffix of scanned characters matches pattern
    template<class Callback>
    void Scan(char character, size_t offset, Callback on_match) {
        state_ = state_.Next(character);
        aho_corasick_automaton_.get()->GenerateMatches(state_,
            [this, offset] (size_t id) mutable {
                if (offset >= id) {
                    ++words_occurrences_by_position_[(offset - id)
                        % pattern_length_];
                }
            });
        if (offset + 1 >= pattern_length_) {
            if (words_occurrences_by_position_[(offset + 1) % pattern_length_]
                     == number_of_words_) {
                on_match();
            }
            words_occurrences_by_position_[(offset + 1) % pattern_length_] = 0;
        }
    }

private:
    std::vector<size_t> words_occurrences_by_position_;
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
        const std::string& text,
        char wildcard) {
    size_t patternLength = patternWithWildcards.length();
    WildcardMatcher matcher;
    matcher.Init(patternWithWildcards, wildcard);
    std::vector<size_t> occurrences;
    for (size_t offset = 0; offset < text.size(); ++offset) {
        matcher.Scan(text[offset], offset,
            [&occurrences, offset, patternLength] () {
                occurrences.push_back(offset + 1 - patternLength);
            });
    }
    return occurrences;
}

template <class Type>
void Print(const std::vector<Type>& sequence) {
    std::cout << sequence.size() << std::endl;
    for (const auto &pos : sequence) {
        std::cout << pos << " ";
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
