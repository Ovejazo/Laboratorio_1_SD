#include <iostream>
#include <vector>
#include <omp.h>

#include "WavePropagation.h"
#include "Network.h"


//Constructor de WavePropagator
WavePropagator::WavePropagator(Network *net, double dt, std::vector<double> src, double initial_energy) 
    : network(net), time_step(dt), sources(src), energy(initial_energy){}
    
//GETTERS
double WavePropagator::GetEnergy(){ return energy; }

//Vamos a calcular la energía
//Sera el calculo serial
void WavePropagator::calculateEnergy(){
    const std::vector<Node>& nodes = network->getNodes();
    this->energy = 0.0;
    for(const Node& node : nodes){
        double amp = node.getAmplitude();
        energy += amp * amp;
    }
}

//Ahora lo vamos a realizar, pero con un metodo
void WavePropagator::calculateEnergy(int method){
    const std::vector<Node>& nodes = network->getNodes();
    this->energy = 0.0;

    if(method == 0){
        #pragma omp parallel for reduction(+:energy) 
        for(const Node& node : nodes){
            double amp = node.getAmplitude();
            energy += amp * amp;
        }
    }
    else if(method == 1){
        #pragma omp parallel for 
        for(const Node& node : nodes){
            double amp = node.getAmplitude();
            #pragma omp atomic 
            energy += amp * amp;
        }
    }
}

//Ahora lo vamos a realizar, pero con un metodo
void WavePropagator::calculateEnergy(int method, bool use_private){
    const std::vector<Node>& nodes = network->getNodes();
    this->energy = 0.0;

    if(method == 0){
        if(use_private){
            #pragma omp parallel
            {
                double local_energy = 0.0;
                #pragma omp for nowait
                for(const Node& node : nodes){
                    double amp = node.getAmplitude();
                    local_energy += amp * amp;
                }
                #pragma omp atomic
                this->energy += local_energy;
            } 
        }else {
            #pragma omp parallel for reduction(+:energy)
            for (int i = 0; i < nodes.size(); ++i) {
                double amp = nodes[i].getAmplitude();
                energy += amp * amp;
            }

        }
    }
    else if(method == 1){
        #pragma omp parallel for 
        for(const Node& node : nodes){
            double amp = node.getAmplitude();
            #pragma omp atomic
            energy += amp * amp;
        }
    }
}

void WavePropagator::processNodes(){
    const std::vector<Node>& nodes = network->getNodes();

    //Vamos a sumar la amplitud de los nodos
    double sum = 0.0;
    for(int i = 0; i < nodes.size(); ++i){
        sum += nodes[i].getAmplitude();
    }
    std::cout << "La suma de las amplitudes es: " << sum << std::endl;
}


void WavePropagator::processNodes(int task_type){

    //definimos el tipo de tarea que se va aplicar
    if (task_type == 0) {

        //Conseguimos los vectores
        std::vector<Node>& nodes = network->getNodes();
        double sum = 0.0;

        //Comienza la paralelización
        #pragma omp parallel
        {

            //Se aplica omp single
            #pragma omp single
            {
                for (int i = 0; i < nodes.size(); ++i) {
                    #pragma omp task shared(nodes, sum)
                    {
                        double local_sum = nodes[i].getAmplitude();
                        #pragma omp atomic
                        sum += local_sum;
                    }
                }
            }
        }
        std::cout << "Suma de amplitudes (tasks): " << sum << std::endl;
    } else if (task_type == 1) {
        // Usar parallel for
        std::vector<Node>& nodes = network->getNodes();
        double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for (int i = 0; i < nodes.size(); ++i) {
            sum += nodes[i].getAmplitude();
        }
        std::cout << "Suma de amplitudes (parallel for): " << sum << std::endl;
    }
}


void WavePropagator::processNodes(int task_type, bool use_single){
    if(use_single){
        //Lo usamos para imprimir
        std::vector<Node>& nodes = network->getNodes();
        double sum = 0.0;
        if(task_type){

            //Tarea con "single"
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for(int i = 0; i < nodes.size(); ++i){
                        #pragma omp task shared(nodes, sum)
                        {
                            double local_sum = nodes[i].getAmplitude();
                            #pragma omp atomic
                            sum += local_sum;
                        }
                    }
                }
            }
        } else{
            //Hacemos un parallel for con single solamente
            #pragma omp parallel for reduction(+:sum)
            for(int i = 0; i < nodes.size(); ++i){
                sum += nodes[i].getAmplitude();
            }
        }
    } else{
        //Hacemos la versión sin single
        processNodes(task_type);
    }
}

void WavePropagator::simulatePhasesBarrier(){
    std::vector<Node>& nodes = network->getNodes();
    std::vector<double> temp(nodes.size(), 0.0);

    #pragma omp parallel
    {
        //Vamos a hacer algún for
        #pragma omp for
        for(int i = 0; i < nodes.size(); i++){
            temp[i] = nodes[i].getAmplitude() * 2;
        }

        //Colocamos la barrera
        #pragma omp barrier

        //Vamos a hacer el segundo for
        #pragma omp for
        for(int i = 0; i < nodes.size(); i++){
            nodes[i].setAmplitude(temp[i]);
        }
    }

}


void WavePropagator::parallelInitializationSingle() {
    std::vector<Node>& nodes = network->getNodes();

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        #pragma omp single
        {
            std::cout << "Soy la hebra, " << tid << " inicializando las hebras." << std::endl;
            for (Node& node : nodes) {
                node.setAmplitude(0.0);
            }
        }
    }
}

void WavePropagator::calculateMetricsFirstprivate() {
    std::vector<Node>& nodes = network->getNodes();
    double offset = 10.0; // Ejemplo: cada hilo parte de este valor

    #pragma omp parallel for firstprivate(offset)
    for (int i = 0; i < nodes.size(); ++i) {
        double value = offset + nodes[i].getAmplitude();
        // Puedes hacer lo que quieras con value, ej: imprimirlo
        // Aquí solo para demostrar el uso de firstprivate
        #pragma omp critical
        {
            //std::cout << "Hilo " << omp_get_thread_num() << " nodo " << i << " valor: " << value << std::endl;
        }
    }
}

void WavePropagator::calculateFinalStateLastprivate() {
    std::vector<Node>& nodes = network->getNodes();
    double last_amplitude = 0.0;

    #pragma omp parallel for lastprivate(last_amplitude)
    for (int i = 0; i < nodes.size(); ++i) {
        last_amplitude = nodes[i].getAmplitude();
    }
    std::cout << "[lastprivate] Última amplitud procesada: " << last_amplitude << std::endl;
}