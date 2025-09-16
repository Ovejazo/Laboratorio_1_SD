#include <iostream>
#include <vector>
#include <fstream>
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
};

// ============================================================================
// CLASE Network
// ============================================================================
class Network {
private:
    std::vector<Node> nodes;        
    int network_size;               
    double diffusion_coeff;         
    double damping_coeff;           

public:
    Network(int size, double diff_coeff, double damp_coeff) 
        : network_size(size), diffusion_coeff(diff_coeff), damping_coeff(damp_coeff) {
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
            const Node& current_node = nodes[i];
            double current_amplitude = current_node.getAmplitude();

            double sum_of_differences = 0.0;
            for (int neighbor_id : current_node.getNeighbors()) {
                const Node& neighbor = nodes[neighbor_id];
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
};

// ============================================================================
// FUNCIÓN PRINCIPAL
// ============================================================================
int main() {
    std::cout << "Iniciando simulador de propagación de ondas (Versión Serial)" << std::endl;
    
    const int num_nodes = 5;
     const double D = 0.5;
    const double gamma = 0;
    const double dt = 0.1;
    const int num_steps = 100;
    
    std::cout << "Parametros: Nodos=" << num_nodes << ", D=" << D << ", gamma=" << gamma;
    std::cout << ", dt=" << dt << ", Pasos=" << num_steps << std::endl;

    Network my_network(num_nodes, D, gamma);
    my_network.initializeLinearNetwork();
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