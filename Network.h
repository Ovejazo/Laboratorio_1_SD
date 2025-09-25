#ifndef NETWORK_H
#define NETWORK_H

#include "Node.h"
#include <vector>
#include <string>


class Network {
private:
    std::vector<Node> nodes;
    int network_size;
    double diffusion_coeff;
    double damping_coeff;

    //En caso de que sea una network 2D
    int ancho_malla = 0.0;
    int alto_malla = 0.0;

    std::vector<double> sources;
    double time_step = 0.0;
    //string tipo_network;

    std::vector<double> scratch_amplitudes;

    bool initialized = false;

public:

    //Constructor
    Network(int size, double diff_coeff, double damp_coeff);
    //Network(int size, double diff_coeff, double damp_coeff, std::vector<double> src, double dt);
    
    //Funciones que piden en el enunciado
    void initializeRandomNetwork();
    void initializeRegularNetwork(int dimensions, int w = 0, int h = 0);

    //GETTERS
    int getSize() const { return network_size; }
    double getDiffusionCoeff() const { return diffusion_coeff;}
    double getDampingCoeff() const { return damping_coeff; } 
    std::vector<Node>& getNodes() {return nodes; }
    const std::vector<Node>& getNodes() const {return nodes; }
    bool inInitialized() const {return initialized;}

    int getAltoMalla() const {return alto_malla;}
    int getAnchoMalla() const {return ancho_malla;}
    Node& getNode(int index);
    std::vector<double> getCurrentAmplitudes() const {
        std::vector<double> out;
        out.reserve(nodes.size());
        for(auto& n : nodes) out.push_back(n.getAmplitude());
        return out;
    };
    //std::string getNetworkType() const;

    //SETTERS
    void setTimeStep(double dt) {time_step = dt;}
    void setSources(const std::vector<double>& src) { sources = src;}
    //void setDimensionesDeMalla(int ancho, int alto);
    //void setNetworkType(const std::string& type);

    //Demás funciones
    void initializeLinearNetwork();
    void initializeGrid2D(int width, int height);
    void initializeRandomNetwork(double connection_probability);
    void initializeSmallWorldNetwork(int k, double beta);
    void verifyGridConnections() const;
    void debugRandomNetwork() const;

    //Metodos obligatorios
    void propagateWaves();
    void propagateWaves(int schedule_type);
    void propagateWaves(int schedule_type, int chunck_size);
    void propagateWavesCollapse();


};

#endif