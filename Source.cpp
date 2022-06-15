// For this code to work C++17 or greater must be used

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// For some weird reason all files have \r on the end of
// each line. Maybe problem only on Windows?
#define NEWLINE_SYMBOL '\r'
#define DELIMITER ","

// Forward-declaration of structures
struct NodeDirected;
struct Edge;

struct Node {
    std::string id;
    std::string label;
    std::vector<Node*> targets;
};

struct Edge {
    NodeDirected* target;
    unsigned int weight;
};

struct NodeDirected {
    std::string id;
    std::string label;
    std::vector<Edge> targets;
};


class UndirectedGraph {
    public:
        std::map<std::string, Node> graph;


        UndirectedGraph(std::string nodes_file_name, std::string edges_file_name) {
            this->nodes_file.open(nodes_file_name);
            this->edges_file.open(edges_file_name);
            this->createNodes();
            this->addEdges();
        }

        ~UndirectedGraph() {
            this->nodes_file.close();
            this->edges_file.close();
        }

        void deletePerson(std::string character_name) {
            const std::string person_id = findID(character_name);

            if(person_id.length() != 0) {
                for(auto& target : this->graph[person_id].targets){
                    auto index = std::find(target->targets.begin(), target->targets.end(), &this->graph[person_id]) - target->targets.begin();
                    target->targets.erase(target->targets.begin() + index);
                }

                this->graph.erase(person_id);
                printf("Person %s was successfully deleted!\n", character_name.c_str());
            }
            else
                printf("No person %s found in a graph!\n", character_name.c_str());
        }

    private:
        std::ifstream nodes_file;
        std::ifstream edges_file;


        void createNodes() {
            std::string line;
            bool isFirstLine = true;

            while(std::getline(this->nodes_file, line, NEWLINE_SYMBOL)) {
                if(isFirstLine) { isFirstLine = false; continue; }
                
                Node node;
                char* buff = new char[line.length() + 1];
                strcpy_s(buff, line.length() + 1, line.c_str());

                char* next_token = nullptr;
                char* token = strtok_s(buff, DELIMITER, &next_token);
                node.id = token;
                node.label = next_token;

                this->graph[node.id] = node;
            }
        }

        void addEdges() {
            std::string line;
            bool isFirstLine = true;

            while(std::getline(this->edges_file, line, NEWLINE_SYMBOL)) {
                if(isFirstLine) { isFirstLine = false; continue; }
                
                char* buff = new char[line.length() + 1];
                strcpy_s(buff, line.length() + 1, line.c_str());

                char* next_token = nullptr;
                char* token = strtok_s(buff, DELIMITER, &next_token);
                
                std::string source = token;
                token = strtok_s(nullptr, DELIMITER, &next_token);
                std::string target = token;

                // As the graph is undirected, nodes must point to each other
                this->graph[source].targets.push_back(&this->graph[target]);
                this->graph[target].targets.push_back(&this->graph[source]);
            }
        }

        std::string findID(std::string name) {
            for(auto& [char_id, node] : this->graph)
                // If we found a person - return its id
                if(strcmp(name.c_str(), node.label.c_str()) == 0)
                    return char_id;

            // Else return empty string
            return "";
        }
};

struct TwoGraphComparison {
    static void deceasedPersons(UndirectedGraph& first_book, UndirectedGraph& second_book) {
        printf("\nDead characters from book 1:\n");

        for(auto& [person_id, node] : first_book.graph)
            // If no person in second graph, print his/her name
            if(second_book.graph.find(person_id) == second_book.graph.end())
                printf("%s\n", node.label.c_str());
    }

    static void newPersons(UndirectedGraph& first_book, UndirectedGraph& second_book) {
        printf("\nNew characters in book 2:\n");
        for(auto& [person_id, node] : second_book.graph)
            // If no person in first graph, print his/her name
            if(first_book.graph.find(person_id) == first_book.graph.end())
                printf("%s\n", node.label.c_str());
    }
};


class DirectedGraph {
    public:
        std::map<std::string, NodeDirected> graph;


        DirectedGraph(std::string nodes_file_name, std::string edges_file_name) {
            this->nodes_file.open(nodes_file_name);
            this->edges_file.open(edges_file_name);
            this->createNodes();
            this->addEdges();
        }

        ~DirectedGraph() {
            this->nodes_file.close();
            this->edges_file.close();
        }

        void printTopWeights() {
            auto all_weights = calculateWeightForEveryone();

            printf("\nTop 10 of all weights:\n");
            for(int i = 0; i < 10; i++) {
                std::string largest = all_weights.begin()->first;
                for(auto& [char_id, weight] : all_weights) {
                    if(weight > all_weights[largest])
                        largest = char_id;
                }
                printf("%s - %u\n", largest.c_str(), all_weights[largest]);
                all_weights.erase(largest);
            }
        }

    private:
        std::ifstream nodes_file;
        std::ifstream edges_file;


        void createNodes() {
            std::string line;
            bool isFirstLine = true;

            while(std::getline(this->nodes_file, line, NEWLINE_SYMBOL)) {
                if(isFirstLine) { isFirstLine = false; continue; }
                
                NodeDirected node;
                char* buff = new char[line.length() + 1];
                strcpy_s(buff, line.length() + 1, line.c_str());

                char* next_token = nullptr;
                char* token = strtok_s(buff, DELIMITER, &next_token);
                node.id = token;
                node.label = next_token;

                this->graph[node.id] = node;
            }
        }

        void addEdges() {
            std::string line;
            bool isFirstLine = true;

            while(std::getline(this->edges_file, line, NEWLINE_SYMBOL)) {
                if(isFirstLine) { isFirstLine = false; continue; }
                
                char* buff = new char[line.length() + 1];
                strcpy_s(buff, line.length() + 1, line.c_str());

                char* next_token = nullptr;
                char* token = strtok_s(buff, DELIMITER, &next_token);
                
                std::string source = token;
                token = strtok_s(nullptr, DELIMITER, &next_token);
                std::string target = token;
                token = strtok_s(nullptr, DELIMITER, &next_token);
                token = strtok_s(nullptr, DELIMITER, &next_token);
                unsigned int weigth = strtol(token, nullptr, 10);

                Edge edge;
                edge.target = &this->graph[target];
                edge.weight = weigth;

                this->graph[source].targets.push_back(edge);
            }
        }
    
        std::map<std::string, unsigned int> calculateWeightForEveryone() {
            std::map<std::string, unsigned int> buff;
            for(auto& [char_id, node] : this->graph) {
                unsigned int total_weight = 0;

                for(auto& target : node.targets)
                    total_weight += target.weight;
                
                buff[char_id] = total_weight;
            }
            return buff;
        }
};


int main() {

    UndirectedGraph first_book("asoiaf-book1-nodes.csv", "asoiaf-book1-edges.csv");
    first_book.deletePerson("Addam Marbrand");

    UndirectedGraph second_book("asoiaf-book2-nodes.csv", "asoiaf-book2-edges.csv");
    // Compare two graphs
    TwoGraphComparison::deceasedPersons(first_book, second_book);
    TwoGraphComparison::newPersons(first_book, second_book);

    // Create directed graph
    DirectedGraph dir_graph("asoiaf-book1-nodes.csv", "asoiaf-book1-edges.csv");
    dir_graph.printTopWeights();

    std::cin.get();
    return 0;
}
