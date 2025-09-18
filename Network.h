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

public:
    Network(int size, double diff_coeff, double damp_coeff);
    void initializeLinearNetwork();
    int getSize() const;
    double getDiffusionCoeff() const;
    double getDampingCoeff() const;
    Node& getNode(int index);
    const std::vector<Node>& getNodes() const;
    void propagateWaveSerial(double time_step, const std::vector<double>& external_sources);
    std::vector<double> getCurrentAmplitudes() const;
    void initializeGrid2D(int width, int height);
    void initializeRandomNetwork(double connection_probability);
    void initializeSmallWorldNetwork(int k, double beta);
    void verifyGridConnections() const;
    void debugRandomNetwork() const;
    void setNetworkType(const std::string& type);
    void setDimensionesDeMalla(int ancho, int alto);
    int getAltoMalla() const;
    int getAnchoMalla() const;
    std::string getNetworkType() const;
};

#endif