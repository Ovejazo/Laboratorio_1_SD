#include <random>
#include <algorithm> 
#include <iostream>
#include <vector>


#include "Network.h"

//Funciones de network
Network::Network(int size, double diff_coeff, double damp_coeff, std::vector<double> src, double dt) 
    :   network_size(size), diffusion_coeff(diff_coeff),
        damping_coeff(damp_coeff), ancho_malla(0), alto_malla(0),
        sources(src), time_step(dt){
    nodes.reserve(size);
    for (int i = 0; i < size; ++i) {
        nodes.emplace_back(i, 0.0);
    }
}

void Network::initializeLinearNetwork() {
    for (int i = 0; i < network_size; ++i) {
        if (i > 0) {
            nodes[i].addNeighbor(i - 1);
        }
        if (i < network_size - 1) {
            nodes[i].addNeighbor(i + 1);
        }
    }
    if (!nodes.empty()) {
        nodes[0].setAmplitude(1.0);
    }
    std::cout << "Red lineal inicializada con " << network_size << " nodos." << std::endl;
}

int Network::getSize() const { return network_size; }
double Network::getDiffusionCoeff() const { return diffusion_coeff; }
double Network::getDampingCoeff() const { return damping_coeff; }
Node& Network::getNode(int index) { return nodes[index]; }
std::vector<Node>& Network::getNodes() { return nodes; }

// Función para obtener todas las amplitudes actuales
std::vector<double> Network::getCurrentAmplitudes() const {
    std::vector<double> amplitudes;
    amplitudes.reserve(network_size);
    for (const Node& node : nodes) {
        amplitudes.push_back(node.getAmplitude());
    }
    return amplitudes;
}

void Network::setDimensionesDeMalla(int ancho, int alto){
    ancho_malla = ancho;
    alto_malla = alto;
}

int Network::getAltoMalla() const { return alto_malla; }
int Network::getAnchoMalla() const { return ancho_malla; }

void Network::initializeGrid2D(int width, int height) {
    network_size = width * height;

    ancho_malla = width;
    alto_malla = height;

    nodes.clear();
    nodes.reserve(network_size);
    
    //Se crean los nodos
    for (int i = 0; i < network_size; ++i) {
        nodes.emplace_back(i, 0.0);
    }
    
    //For para conectar los vecinos de forma 2D
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int current_id = y * width + x;
            
            // Conectar con vecino izquierdo (si existe)
            if (x > 0) {
                nodes[current_id].addNeighbor(y * width + (x - 1));
            }
            // Conectar con vecino derecho (si existe)
            if (x < width - 1) {
                nodes[current_id].addNeighbor(y * width + (x + 1));
            }
            // Conectar con vecino arriba (si existe)
            if (y > 0) {
                nodes[current_id].addNeighbor((y - 1) * width + x);
            }
            // Conectar con vecino abajo (si existe)
            if (y < height - 1) {
                nodes[current_id].addNeighbor((y + 1) * width + x);
            }
        }
    }
    

        int center_x = width / 2;
        int center_y = height / 2;
        int center_id = center_y * width + center_x;
        nodes[center_id].setAmplitude(1.0);

        tipo_de_network = "grid2D";
    };

void Network::initializeSmallWorldNetwork(int k, double beta) {
    // Primero crear una red regular (anillo)
    nodes.clear();
    nodes.reserve(network_size);
    for (int i = 0; i < network_size; ++i) {
        nodes.emplace_back(i, 0.0);
    }
    
    // Conectar cada nodo con sus k vecinos más cercanos
    for (int i = 0; i < network_size; ++i) {
        for (int j = 1; j <= k/2; ++j) {
            int left = (i - j + network_size) % network_size;
            int right = (i + j) % network_size;
            nodes[i].addNeighbor(left);
            nodes[i].addNeighbor(right);
        }
    }
    
    // Rewiring aleatorio con probabilidad beta
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::uniform_int_distribution<> node_dis(0, network_size - 1);
    
    for (int i = 0; i < network_size; ++i) {
        for (int j = 1; j <= k/2; ++j) {
            if (dis(gen) < beta) {
                int old_neighbor = (i + j) % network_size;
                
                // Remover la conexión existente
                nodes[i].removeNeighbor(old_neighbor);
                nodes[old_neighbor].removeNeighbor(i);
                
                // Elegir un nuevo vecino aleatorio (que no sea ya vecino)
                int new_neighbor;
                do {
                    new_neighbor = node_dis(gen);
                } while (new_neighbor == i || nodes[i].isNeighbor(new_neighbor));
                
                // Crear nueva conexión
                nodes[i].addNeighbor(new_neighbor);
                nodes[new_neighbor].addNeighbor(i);
            }
        }
    }
    

    //Agregamos la perturbación inicial 
    int nodo_central = network_size / 2;
    nodes[nodo_central].setAmplitude(1.0);

    tipo_de_network = "small_world";

    };


void Network::initializeRandomNetwork(double connection_probability) {
    // Crear nodos
    for (int i = 0; i < network_size; ++i) {
        nodes.emplace_back(i, 0.0);
    }
    
    // Conexiones aleatorias
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < network_size; ++i) {
        for (int j = i + 1; j < network_size; ++j) {
            if (dis(gen) < connection_probability) {
                nodes[i].addNeighbor(j);
                nodes[j].addNeighbor(i);  // Para una red no dirigida
            }
        }
    }
    
    //Perturbación inicial
    int nodo_central = network_size/2;
    nodes[nodo_central].setAmplitude(1.0);


    tipo_de_network = "random";
};

void Network::verifyGridConnections() const {
    std::cout << "=== VERIFICACIÓN CONEXIONES GRID 2D ===" << std::endl;
    std::cout << "Dimensiones: " << ancho_malla << "x" << alto_malla << std::endl;
    
    for (int y = 0; y < alto_malla; ++y) {
        for (int x = 0; x < ancho_malla; ++x) {
            int id = y * ancho_malla + x;
            const Node& node = nodes[id];
            
            std::cout << "Nodo (" << x << "," << y << ") ID=" << id 
                      << " -> " << node.getDegree() << " vecinos: ";
            
            for (int neighbor_id : node.getNeighbors()) {
                // Convertir ID de vuelta a coordenadas para verificar
                int nx = neighbor_id % ancho_malla;
                int ny = neighbor_id / ancho_malla;
                std::cout << "(" << nx << "," << ny << ") ";
            }
            std::cout << std::endl;
        }
    }
};

void Network::debugRandomNetwork() const {
    std::cout << "=== DEBUG RED ALEATORIA ===" << std::endl;
    std::cout << "Número de nodos: " << network_size << std::endl;
    
    // Contar conexiones totales
    int total_connections = 0;
    for (int i = 0; i < network_size; ++i) {
        total_connections += nodes[i].getDegree();
    }
    
    double avg_degree = (double)total_connections / network_size;
    std::cout << "Conexiones totales: " << total_connections << std::endl;
    std::cout << "Grado promedio: " << avg_degree << std::endl;
    
    // Verificar perturbación inicial
    int center_node = network_size / 2;
    std::cout << "Nodo central (" << center_node << ") amplitud inicial: " 
              << nodes[center_node].getAmplitude() << std::endl;
    
    // Mostrar vecinos del nodo central
    std::cout << "Vecinos del nodo central: ";
    for (int neighbor_id : nodes[center_node].getNeighbors()) {
        std::cout << neighbor_id << " ";
    }
    std::cout << std::endl;
    
    // Mostrar algunos nodos de ejemplo
    std::cout << "Grados de algunos nodos: ";
    for (int i = 0; i < std::min(10, network_size); i += network_size/10) {
        std::cout << "N" << i << "(" << nodes[i].getDegree() << ") ";
    }
    std::cout << std::endl;
};


// ---------------------------- REALIZAMOS LAS FUNCIONES OBLIGATORIAS ---------------------------- 
void Network::propagateWaves() {
    // Versión serial
    // Código similar a tu integrateEuler, pero que opere sobre los nodos

    //Definimos las variables que vamos a usar
    int network_size = this->getSize();
    double diffusion_coeff = this->getDiffusionCoeff();
    double damping_coeff = this->getDampingCoeff();
    std::vector<Node>& nodes = this->getNodes();

    std::vector<double> new_amplitudes(network_size, 0.0);

    for (int i = 0; i < network_size; ++i) {

        //obtenemos el nodo actual
        const Node& current_node = nodes[i];
        
        //Conseguimos la amplitud el nodo
        double current_amplitude = current_node.getAmplitude();

        //Comenzamos a sumar las diferencias
        double sum_of_differences = 0.0;

        //Vemos los nodos vecinos del nodo que estamos viendo
        for (int neighbor_id : current_node.getNeighbors()) {

            //Conseguimos el nodo vecino de los nodos vecinos
            const Node& neighbor = nodes[neighbor_id];

            //Calculamos la diferencia de las amplitudes 
            sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
        }

        double diffusion_term = diffusion_coeff * sum_of_differences;
        double damping_term = -damping_coeff * current_amplitude;
        double source_term = (i < sources.size()) ? sources[i] : 0.0;

        double total_change = time_step * (diffusion_term + damping_term + source_term);

        new_amplitudes[i] = current_amplitude + total_change;
    }

    for (int i = 0; i < network_size; ++i) {
        nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
        nodes[i].setAmplitude(new_amplitudes[i]);
    }

}
void Network::propagateWaves(int schedule_type){

    //Definimos las variables que vamos a usar
    int network_size = this->getSize();
    double diffusion_coeff = this->getDiffusionCoeff();
    double damping_coeff = this->getDampingCoeff();
    std::vector<Node>& nodes = this->getNodes();

    std::vector<double> new_amplitudes(network_size, 0.0);

    switch (schedule_type) {
        case 0:
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(static)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
        case 1:
            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
        case 2:
            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
    }
}




void Network::propagateWaves(int schedule_type, int chunck_size){

    //Definimos las variables que vamos a usar
    int network_size = this->getSize();
    double diffusion_coeff = this->getDiffusionCoeff();
    double damping_coeff = this->getDampingCoeff();
    std::vector<Node>& nodes = this->getNodes();

    std::vector<double> new_amplitudes(network_size, 0.0);

    switch (schedule_type) {
        case 0:
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(static)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
        case 1:
            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
        case 2:
            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < network_size; ++i) {

                //obtenemos el nodo actual
                const Node& current_node = nodes[i];
                
                //Conseguimos la amplitud el nodo
                double current_amplitude = current_node.getAmplitude();

                //Comenzamos a sumar las diferencias
                double sum_of_differences = 0.0;

                //Vemos los nodos vecinos del nodo que estamos viendo
                for (int neighbor_id : current_node.getNeighbors()) {

                    //Conseguimos el nodo vecino de los nodos vecinos
                    const Node& neighbor = nodes[neighbor_id];

                    //Calculamos la diferencia de las amplitudes 
                    sum_of_differences += (neighbor.getAmplitude() - current_amplitude);
                }

                double diffusion_term = diffusion_coeff * sum_of_differences;
                double damping_term = -damping_coeff * current_amplitude;
                double source_term = (i < sources.size()) ? sources[i] : 0.0;

                double total_change = time_step * (diffusion_term + damping_term + source_term);

                new_amplitudes[i] = current_amplitude + total_change;
            }

            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
    }
}



void Network::propagateWavesCollapse() {
    // Usa #pragma omp for collapse(2) para loops anidados, por ejemplo para grillas 2D
}