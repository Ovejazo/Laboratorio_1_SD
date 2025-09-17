#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm> 
#include <iomanip>

// ============================================================================
// CLASE Node
// ============================================================================
class Node {
private:
    int id;                         
    double amplitude;               
    double previous_amplitude;      
    std::vector<int> neighbors;     

public:
    Node(int node_id, double initial_amplitude = 0.0) 
        : id(node_id), amplitude(initial_amplitude), previous_amplitude(initial_amplitude) {}

    int getId() const { return id; }
    double getAmplitude() const { return amplitude; }
    double getPreviousAmplitude() const { return previous_amplitude; }
    const std::vector<int>& getNeighbors() const { return neighbors; }
    int getDegree() const { return neighbors.size(); }

    void setAmplitude(double new_amplitude) { amplitude = new_amplitude; }
    void setPreviousAmplitude(double prev_amp) { previous_amplitude = prev_amp; }

    void addNeighbor(int neighbor_id) {
        neighbors.push_back(neighbor_id);
    }

    void removeNeighbor(int neighbor_id) {
        // Buscar el vecino y eliminarlo si existe
        auto it = std::find(neighbors.begin(), neighbors.end(), neighbor_id);
        if (it != neighbors.end()) {
            neighbors.erase(it);
        }
    }

    bool isNeighbor(int node_id) const {
        return std::find(neighbors.begin(), neighbors.end(), node_id) != neighbors.end();
    }
};

// --------------------------------- CLASE NETWORK ---------------------------------
class Network {
private:

//Parametros de network
    std::vector<Node> nodes;   
    int network_size;               
    double diffusion_coeff;         
    double damping_coeff;  
    std::string tipo_de_network;
    
    //Para la dimensión 2D
    int ancho_malla;
    int alto_malla;         

public:

//Funciones de network
    Network(int size, double diff_coeff, double damp_coeff) 
        : network_size(size), diffusion_coeff(diff_coeff),
          damping_coeff(damp_coeff), ancho_malla(0), alto_malla(0) {
        nodes.reserve(size);
        for (int i = 0; i < size; ++i) {
            nodes.emplace_back(i, 0.0);
        }
    }

    void initializeLinearNetwork() {
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

    int getSize() const { return network_size; }
    double getDiffusionCoeff() const { return diffusion_coeff; }
    double getDampingCoeff() const { return damping_coeff; }
    Node& getNode(int index) { return nodes[index]; }
    const std::vector<Node>& getNodes() const { return nodes; }

    void propagateWaveSerial(double time_step, const std::vector<double>& external_sources) {
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
            double source_term = (i < external_sources.size()) ? external_sources[i] : 0.0;
            double total_change = time_step * (diffusion_term + damping_term + source_term);

            new_amplitudes[i] = current_amplitude + total_change;
        }

        for (int i = 0; i < network_size; ++i) {
            nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
            nodes[i].setAmplitude(new_amplitudes[i]);
        }
    }

    // Función para obtener todas las amplitudes actuales
    std::vector<double> getCurrentAmplitudes() const {
        std::vector<double> amplitudes;
        amplitudes.reserve(network_size);
        for (const Node& node : nodes) {
            amplitudes.push_back(node.getAmplitude());
        }
        return amplitudes;
    }

    // Nuevos métodos de inicialización
    void initializeGrid2D(int width, int height);
    void initializeRandomNetwork(double connection_probability);
    void initializeSmallWorldNetwork(int k, double beta);

    // Metodos para verificar
    void verifyGridConnections() const; 
    
    // Método para seleccionar tipo de red
    void setNetworkType(const std::string& type);

    void setDimensionesDeMalla(int ancho, int alto){
        ancho_malla = ancho;
        alto_malla = alto;
    }

    int getAltoMalla() const { return alto_malla; }
    int getAnchoMalla() const { return ancho_malla; }

    
    // Getter para el tipo de red
    std::string getNetworkType() const;
};  

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

//------------------------------ MAIN --------------------------------------
int main() {
    std::cout << "Iniciando simulador de propagación de ondas (Versión Serial)" << std::endl;
    
    //Inicializamos los parametros con los que vamos a trabajar
    const int num_nodes = 100;
    const double D = 1;
    const double gamma = 0.01;
    const double dt = 0.1;
    const int num_steps = 100;
    
    std::cout << "Parametros: Nodos=" << num_nodes << ", D=" << D << ", gamma=" << gamma;
    std::cout << ", dt=" << dt << ", Pasos=" << num_steps << std::endl;

    //Creamos nuestra network con la que vamos a trabajar con sus coeficientes
    Network my_network(num_nodes, D, gamma);

    // Seleccionar tipo de red (podría ser un parámetro de entrada)
    std::string network_type = "grid2D";  // o "linear", "random", "small_world"
    
    if (network_type == "linear") {
        my_network.initializeLinearNetwork();
    } 
    else if (network_type == "grid2D") {
        int width = 10;  // por ejemplo
        int height = 10;
        my_network.initializeGrid2D(width, height);
        my_network.verifyGridConnections(); 

        std::cout << "Se salio de la inicialización 2D" << std::endl;
        std::cout << ", dt=" << dt << ", Pasos=" << num_steps << std::endl;
    }
    else if (network_type == "random") {
        double connection_prob = 0.1;  // 10% de probabilidad de conexión
        my_network.initializeRandomNetwork(connection_prob);
    }
    else if (network_type == "small_world") {
        int k = 4;       // cada nodo conectado a k vecinos más cercanos
        double beta = 0.1;  // probabilidad de rewiring
        my_network.initializeSmallWorldNetwork(k, beta);
    }
    
    std::vector<double> sources(num_nodes, 0.0);

    // 1. ABRIR ARCHIVO CSV PARA ESCRITURA (EVOLUCIÓN COMPLETA)
    std::ofstream output_file("results.csv");
    if (!output_file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo results.csv" << std::endl;
        return 1;
    }

    // 2. ESCRIBIR CABECERA (Time_Step + Node_0, Node_1, ..., Node_9)
    output_file << "Time_Step";
    for (int i = 0; i < num_nodes; ++i) {
        output_file << ",Node_" << i;
    }
    output_file << "\n";

    // 3. ESCRIBIR ESTADO INICIAL (Paso 0)
    output_file << "0";
    std::vector<double> initial_amplitudes = my_network.getCurrentAmplitudes();
    for (double amp : initial_amplitudes) {
        output_file << "," << std::scientific << std::setprecision(6) << amp;
    }
    output_file << "\n";

    // 4. BUCLE PRINCIPAL DE SIMULACIÓN
    for (int step = 1; step <= num_steps; ++step) {
        my_network.propagateWaveSerial(dt, sources);

        // Escribir resultados de este paso en el CSV
        output_file << step;
        std::vector<double> current_amplitudes = my_network.getCurrentAmplitudes();
        for (double amp : current_amplitudes) {
            output_file << "," << std::scientific << std::setprecision(6) << amp;
        }
        output_file << "\n";

        // Imprimir en consola cada 25 pasos
        if (step % 25 == 0) {
            std::cout << "Paso " << step << ": ";
            for (int i = 0; i < num_nodes; ++i) {
                std::cout << my_network.getNode(i).getAmplitude() << " ";
            }
            std::cout << std::endl;
        }
    }

    // 5. CERRAR ARCHIVO
    output_file.close();
    std::cout << "Datos de evolucion guardados en 'results.csv'." << std::endl;

    std::cout << "Estado final: ";
    for (int i = 0; i < num_nodes; ++i) {
        std::cout << my_network.getNode(i).getAmplitude() << " ";
    }
    std::cout << std::endl;

    std::cout << "Simulación serial completada exitosamente." << std::endl;
    return 0;
}