#include <random>
#include <algorithm> 
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

#include "Network.h"

//Funciones de network
/*
metodo: Network
descripcion: 
retorno: -
*/
Network::Network(int size, double diff_coeff, double damp_coeff) 
    :   network_size(size),
        diffusion_coeff(diff_coeff),
        damping_coeff(damp_coeff),
        ancho_malla(0),
        alto_malla(0),
        initialized(false),
        time_step(0.0),
        current_time(0.0),
        source_mode(SourceMode::Fixed),
        source_amplitude(0.0),
        source_omega(0.0)
{
        nodes.reserve(network_size);
        for(int i = 0; i < network_size; ++i){
            nodes.emplace_back(i, 0.0);
        }
        sources.assign(network_size, 0.0);
        scratch_amplitudes.assign(network_size, 0.0);
}

/*
metodo: initializeREgularNetwork
descripcion: 
retorno: -
*/
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
    current_time = 0.0;
    scratch_amplitudes.assign(network_size, 0.0);
}

/*
metodo: setSources
descripcion: 
retorno: -
*/
void Network::setSources(const std::vector<double>& src){
    sources = src;
    if ((int)sources.size() != network_size){
        sources.resize(network_size, 0.0);
    }
    source_mode = SourceMode::Fixed;
}

/*
metodo: setZeroSource
descripcion: 
retorno: -
*/
void Network::setZeroSource(){
    sources.assign(network_size, 0.0);
    source_mode = SourceMode::Zero;
}

/*
metodo: generateRandomSources
descripcion: 
retorno: -
*/
void Network::generateRandomSources(double min_value, double max_value, unsigned int seed){
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(min_value, max_value);
    sources.resize(network_size);

    for(int i = 0; i < network_size; ++i){
        sources[i] = dist(rng);
    }

    source_mode = SourceMode::Random;
}

/*
metodo: setSineSource
descripcion: 
retorno: -
*/
void Network::setSineSource(double amplitude, double omega){
    source_amplitude = amplitude;
    source_omega = omega;
    source_mode = SourceMode::Sine_uniform;
}

/*
metodo: evalSourceTerm
descripcion: 
retorno: -
*/
inline double Network::evalSourceTerm(int i, double t) const {
    switch (source_mode) {
        case SourceMode::Zero:
            return 0.0;
        case SourceMode::Fixed:
            return (i < (int)sources.size()) ? sources[i] : 0.0;
        case SourceMode::Random:
            return (i < (int)sources.size()) ? sources[i] : 0.0;
        case SourceMode::Sine_uniform:
            return source_amplitude * std::sin(source_omega * t);
        default:
            return 0.0;
    }
}

/*
metodo: propagateWaves
descripcion: 
retorno: -
*/
void Network::propagateWaves(){
    propagateCore(0, 0, false);
}

/*
metodo: propagateWaves
descripcion: 
retorno: -
*/
void Network::propagateWaves(int schedule_type){
    propagateCore(schedule_type, 0, false);
}

/*
metodo: propagateWaves
descripcion: 
retorno: -
*/
void Network::propagateWaves(int schedule_type, int chunk_size){
    propagateCore(schedule_type, chunk_size, true);
}

/*
metodo: propagateCore
descripcion: 
retorno: -
*/
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

    const double t_now = current_time;

    //Aquí esta el loop principal el cual calcular nuevas amplitudes.
    auto computeBody = [&](int i){ 
        const Node& node = nodes[i];
        double A = node.getAmplitude();
        double sum_diff = 0.0;
        for(int nb : node.getNeighbors()){
            sum_diff += (nodes[nb].getAmplitude() - A);
        }
        double source_term = evalSourceTerm(i, t_now);
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

    current_time += time_step;
}

/*
metodo: propagateWavesCollapse
descripcion: 
retorno: -
*/
void Network::propagateWavesCollapse(){

    if(!initialized){
        std::cerr << "Se inicializo la red antes de ejectura\n";
    }
    if (time_step <= 0.0) {
        std::cerr << "Los pasos no han sido configurados\n";
        time_step = 0.01;
    }
    if (ancho_malla <= 0 || alto_malla <= 0 || ancho_malla * alto_malla != network_size) {
        std::cerr << "[propagateWavesCollapse] La red no fue inicializada en 2D correctamente.\n";
        return;
    }

    const int W = ancho_malla;
    const int H = alto_malla;
    const double D = diffusion_coeff;
    const double gamma = damping_coeff;

    std::vector<double> new_amplitude(network_size, 0.0);
    auto idx = [&](int r, int c){ return r*W + c; };
    const double t_now = current_time;

    #pragma omp parallel for collapse(2) schedule(static)
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            int i = idx(r, c);
            const Node& node = nodes[i];
            double A = node.getAmplitude();
            double sum_diff = 0.0;
            for (int nb : node.getNeighbors()){
                sum_diff += (nodes[nb].getAmplitude() - A);
            }
            double source_term = evalSourceTerm(i, t_now);
            double delta = time_step * (D * sum_diff - gamma * A + source_term);
            new_amplitude[i] = A + delta;
        }
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < network_size; ++i){
        nodes[i].setPreviousAmplitude(nodes[i].getAmplitude());
        nodes[i].setAmplitude(new_amplitude[i]);
    }

    current_time += time_step;
}

/*
metodo: getNode
descripcion: 
retorno: -
*/
Node& Network::getNode(int i){ return this->nodes[i]; }