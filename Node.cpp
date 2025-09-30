#include <vector>
#include <algorithm> 

#include "Node.h"


//Constructor del nodo
Node::Node(int node_id, double initial_amplitude) 
    : id(node_id), amplitude(initial_amplitude), previous_amplitude(initial_amplitude) {}

//Getters
int Node::getId() const { return id; }
double Node::getAmplitude() const { return amplitude; }
double Node::getPreviousAmplitude() const { return previous_amplitude; }
const std::vector<int>& Node::getNeighbors() const { return neighbors; }
int Node::getDegree() const { return neighbors.size(); }


//Setter
void Node::setAmplitude(double new_amplitude) { amplitude = new_amplitude; }
void Node::setPreviousAmplitude(double prev_amp) { previous_amplitude = prev_amp; }

//Con esto agregamos los nodos
void Node::addNeighbor(int neighbor_id) {

    neighbors.push_back(neighbor_id);
}

/*
void Node::removeNeighbor(int neighbor_id) {
    // Buscar el vecino y eliminarlo si existe
    auto it = std::find(neighbors.begin(), neighbors.end(), neighbor_id);
    if (it != neighbors.end()) {
        neighbors.erase(it);
    }
}
*/

bool Node::isNeighbor(int node_id) const {
    return std::find(neighbors.begin(), neighbors.end(), node_id) != neighbors.end();
}