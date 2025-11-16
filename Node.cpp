#include <vector>
#include <algorithm> 

#include "Node.h"


/*
metodo: node
descripcion: constructor del elemento nodo,,,, ellll cual posee su id, amplitud actual y amplitud previa
retorno: un nodo
*/
Node::Node(int node_id, double initial_amplitude) 
    : id(node_id), amplitude(initial_amplitude), previous_amplitude(initial_amplitude) {}

/*
metodo: getId
descripcion: obtiene el id de un nodo
retorno: entero id del nodo
*/
int Node::getId() const { return id; }

/*
metodo: getAmplitude
descripcion: obtiene la amplitud actual de un nodo
retorno: -
*/
double Node::getAmplitude() const { return amplitude; }

/*
metodo: getPreviousAmplitude
descripcion: 
retorno: double amplitud que posee un nodo
*/
double Node::getPreviousAmplitude() const { return previous_amplitude; }


/*
metodo: getNeighbors
descripcion: obtiene todos los nodos vecinos
retorno: -
*/
const std::vector<int>& Node::getNeighbors() const { return neighbors; }

/*
metodo: getDegree
descripcion: 
retorno: entero grado de un nodo
*/
int Node::getDegree() const { return neighbors.size(); }


/*
metodo: setAmplitude
descripcion: coloca la nueva amplitud
retorno: -
*/
void Node::setAmplitude(double new_amplitude) { amplitude = new_amplitude; }

/*
metodo: setPreviousAmplitude
descripcion: 
retorno: -
*/
void Node::setPreviousAmplitude(double prev_amp) { previous_amplitude = prev_amp; }

/*
metodo: addNeighbor
descripcion: Agregar nodos vecinos a un nodo especifico
retorno: -
*/
void Node::addNeighbor(int neighbor_id) {

    neighbors.push_back(neighbor_id);
}


/*
metodo: isNeighbor
descripcion: encargado de verificar si un par de nodos son vecinos
retorno: booleano
*/
bool Node::isNeighbor(int node_id) const {
    return std::find(neighbors.begin(), neighbors.end(), node_id) != neighbors.end();
}