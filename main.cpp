#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "WavePropagation.h"

//------------------------------ MAIN --------------------------------------
int main() {
    std::cout << "Iniciando simulador de propagación de ondas (Versión Serial)" << std::endl;
    
    //Inicializamos los parametros con los que vamos a trabajar
    const int num_nodes = 50;
    const double D = 20;
    const double gamma = 0.01;
    const double dt = 0.01;
    const int num_steps = 1000;
    
    std::cout << "Parametros: Nodos=" << num_nodes << ", D=" << D << ", gamma=" << gamma;
    std::cout << ", dt=" << dt << ", Pasos=" << num_steps << std::endl;

    //Se crea una red que se llamara "my_network"
    Network my_network(num_nodes, D, gamma);

    // Seleccionar tipo de red (podría ser un parámetro de entrada)
    std::string network_type = "linear";  // o "linear", "random", "small_world"
    
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
        my_network.debugRandomNetwork(); 
    }

    else if (network_type == "small_world") {
        int k = 4;       // cada nodo conectado a k vecinos más cercanos
        double beta = 0.1;  // probabilidad de rewiring
        my_network.initializeSmallWorldNetwork(k, beta);
    }
    

    //Fuente externa
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

    //Creamos la propagación
    WavePropagator propagation(&my_network, dt, sources);

    // 4. BUCLE PRINCIPAL DE SIMULACIÓN
    for (int step = 1; step <= num_steps; ++step) {

        // schedule_type: 0=static, 1=dynamic, 2=guided
        propagation.integrateEuler(2);

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