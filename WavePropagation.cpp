#include <vector>
#include <omp.h>

#include "WavePropagation.h"
#include "Network.h"


//Constructor de WavePropagator
WavePropagator::WavePropagator(Network *net, double dt, std::vector<double> src) 
    : network(net), time_step(dt), sources(src){}
    

void WavePropagator::integrateEuler(){

    //Definimos las variables que vamos a usar
    int network_size = network->getSize();
    double diffusion_coeff = network->getDiffusionCoeff();
    double damping_coeff = network->getDampingCoeff();
    std::vector<Node>& nodes = network->getNodes();

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

void WavePropagator::integrateEuler(int sync_type){

    //Definimos las variables que vamos a usar
    int network_size = network->getSize();
    double diffusion_coeff = network->getDiffusionCoeff();
    double damping_coeff = network->getDampingCoeff();
    std::vector<Node>& nodes = network->getNodes();

    std::vector<double> new_amplitudes(network_size, 0.0);

    switch (sync_type) {
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

void WavePropagator::integrateEuler(int sync_type, bool use_barrier){

    //Definimos las variables que vamos a usar
    int network_size = network->getSize();
    double diffusion_coeff = network->getDiffusionCoeff();
    double damping_coeff = network->getDampingCoeff();
    std::vector<Node>& nodes = network->getNodes();

    std::vector<double> new_amplitudes(network_size, 0.0);

    switch (sync_type) {
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

            if(use_barrier){
                #pragma omp barrier
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

            if(use_barrier){
                #pragma omp barrier
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

            if(use_barrier){
                #pragma omp barrier
            }

            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < network_size; ++i) {
                nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
                nodes[i].setAmplitude(new_amplitudes[i]);
            }
            break;
    }
}