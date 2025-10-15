#ifndef NETWORK_H
#define NETWORK_H

#include "Node.h"
#include <vector>
#include <string>


class Network {
public: 
    enum class SourceMode{
        Zero = 0,
        Fixed = 1,
        Random = 2,
        Sine_uniform = 3
    };

private:
    std::vector<Node> nodes;
    int network_size;
    double diffusion_coeff;
    double damping_coeff;

    //En caso de que sea una network 2D
    int ancho_malla = 0;
    int alto_malla = 0;

    bool initialized = false;

    std::vector<double> sources;
    double time_step = 0.0;
    double current_time = 0.0;
    //string tipo_network;

    //Configuración de fuente
    SourceMode source_mode = SourceMode::Fixed;
    double source_amplitude = 0.0;
    double source_omega = 0.0;

    std::vector<double> scratch_amplitudes;



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
    bool isInitialized() const {return initialized;}

    int getAltoMalla() const {return alto_malla;}
    int getAnchoMalla() const {return ancho_malla;}
    Node& getNode(int index);
    std::vector<double> getCurrentAmplitudes() const {
        std::vector<double> out;
        out.reserve(nodes.size());
        for(auto& n : nodes) out.push_back(n.getAmplitude());
        return out;
    };

    double getCurrentTime() const {return current_time;}
    SourceMode getSourceMode() const {return source_mode;}
    //std::string getNetworkType() const;

    //SETTERS
    void setTimeStep(double dt) {time_step = dt;}
    void setSources(const std::vector<double>& src);
    void setZeroSource();
    void generateRandomSources(double min_value, double max_value, unsigned int seed = 5489u);
    void setSineSource(double amplitude, double omega); // S(t)=A sin(ωt)
    void setSourceMode(SourceMode mode) { source_mode = mode; }
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
    void propagateWaves(int schedule_type, int chunk_size);
    void propagateWavesCollapse();

private:
    void propagateCore(int schedule_type, int chunk_size, bool use_chunk);
    inline double evalSourceTerm(int i, double t) const;
};

#endif