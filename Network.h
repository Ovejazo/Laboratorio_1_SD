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
    std::string tipo_de_network;
    int ancho_malla;
    int alto_malla;
    std::vector<double> sources;
    double time_step;

public:

    //Constructor
    Network(int size, double diff_coeff, double damp_coeff, std::vector<double> src, double dt);
    
    //GETTERS
    int getSize() const;
    int getAltoMalla() const;
    int getAnchoMalla() const;
    double getDiffusionCoeff() const;
    double getDampingCoeff() const;
    std::vector<Node>& getNodes();
    Node& getNode(int index);
    std::vector<double> getCurrentAmplitudes() const;
    std::string getNetworkType() const;

    //SETTERS
    void setDimensionesDeMalla(int ancho, int alto);
    void setNetworkType(const std::string& type);

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