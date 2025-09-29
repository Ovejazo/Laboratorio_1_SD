#include <random>
#include <algorithm> 
#include <iostream>
#include <stdexcept>
#include <vector>


#include "Network.h"

//Funciones de network
Network::Network(int size, double diff_coeff, double damp_coeff) 
    :   network_size(size),
        diffusion_coeff(diff_coeff),
        damping_coeff(damp_coeff),
        ancho_malla(0),
        alto_malla(0),
        initialized(false)
{
        nodes.reserve(network_size);
        for(int i = 0; i < network_size; ++i){
            nodes.emplace_back(i, 0.0);
        }
        scratch_amplitudes.assign(network_size, 0.0);
}

void Network::initializeRegularNetwork(int dimensions, int w, int h){
    if (dimensions == 1){

        //Como es unidimensional, sera lineal
        for (int i = 0; i < network_size; ++i) {
            if (i > 0) nodes[i].addNeighbor(i - 1);
            if (i < network_size - 1) nodes[i].addNeighbor(i + 1);
        }

    }else if (dimensions == 2){
        if(h*w != network_size){
            throw std::runtime_error("Dimensiones erroneas");
        }

        //Definimos el alto y el largo
        alto_malla = h;
        ancho_malla = w;

        //Hasta 4 vecinos posiblemente conectados
        auto idx = [w](int r, int c){return r*w+c; };
        for(int r = 0; r < alto_malla; ++r){
            for(int c = 0; c < ancho_malla; ++c){
                int id = idx(r,c);
                if (r > 0) nodes[id].addNeighbor(idx(r-1,c));
                if (r < alto_malla - 1) nodes[id].addNeighbor(idx(r+1,c));
                if (c > 0) nodes[id].addNeighbor(idx(r,c-1));
                if (c < ancho_malla - 1) nodes[id].addNeighbor(idx(r,c+1));
            }
        }
    } else {
        throw std::runtime_error("Solo se soporta 2 dimensiones...");
    }

    initialized = true;
    scratch_amplitudes.assign(network_size, 0.0);
}

void Network::propagateWaves(){
    propagateCore(0, 0, false);
}

void Network::propagateWaves(int schedule_type){
    propagateCore(schedule_type, 0, false);
}

void Network::propagateWaves(int schedule_type, int chunk_size){
    propagateCore(schedule_type, chunk_size, true);
}

void Network::propagateCore(int schedule_type, int chunk_size, bool use_chunk){
    //Vamos a imprimir un mensaje de que entro a la función
    if(!initialized){
        std::cerr << "Se llamo la función antes de iniciar\n";
    }
    if(time_step <= 0.0){
        std::cerr << "Los pasos no han sido configurados\n";
        time_step = 0.01;
    }

    //Definimos las variables que vamos a usarç
    const int N = network_size;
    const double D = diffusion_coeff;
    const double gamma = damping_coeff;
    std::vector<double> new_amplitude(N, 0.0);

    //Aquí esta el loop principal el cual calcular nuevas amplitudes.
    auto computeBody = [&](int i){ 
        const Node& node = nodes[i];
        double A = node.getAmplitude();
        double sum_diff = 0.0;
        for(int nb : node.getNeighbors()){
            sum_diff += (nodes[nb].getAmplitude() - A);
        }
        double source_term = (i < (int)sources.size()) ? sources[i] : 0.0;
        double delta = time_step * (D * sum_diff - gamma * A + source_term);
        new_amplitude[i] = A + delta;
    };

    //Aquí comenzamos la paralelización
    if (use_chunk && chunk_size > 0) {
        switch (schedule_type) {
            case 0: // static, chunk
                #pragma omp parallel for schedule(static, chunk_size)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            case 1: // dynamic, chunk
                #pragma omp parallel for schedule(dynamic, chunk_size)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            case 2: // guided, chunk
                #pragma omp parallel for schedule(guided, chunk_size)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            default:
                #pragma omp parallel for schedule(static, chunk_size)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
        }
    } else {
        switch (schedule_type) {
            case 0: // static
                #pragma omp parallel for schedule(static)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            case 1: // dynamic
                #pragma omp parallel for schedule(dynamic)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            case 2: // guided
                #pragma omp parallel for schedule(guided)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
            default:
                #pragma omp parallel for schedule(static)
                for (int i = 0; i < N; ++i) computeBody(i);
                break;
        }
    }

    // Fase de escritura separada
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i){
        nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
        nodes[i].setAmplitude(new_amplitude[i]);
    }
}

Node& Network::getNode(int i){ return this->nodes[i]; }