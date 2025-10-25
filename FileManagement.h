#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <fstream>
#include <vector>

class Network;
class WavePropagator;

class FileManagement {
public:

    static void crearCarpeta();
    static bool openOutFiles(std::ofstream& csv, std::ofstream& wave_dat, std::ofstream& energy_dat);

    static void writeHeader(std::ofstream& csv,
                            std::ofstream& wave_dat,
                            std::ofstream& energy_dat,
                            int num_nodes);
    
    static void writeInitialState(Network& myNetwork,
                                    WavePropagator& propagation,
                                    std::ofstream& csv,
                                    std::ofstream& wave_dat,
                                    std::ofstream& energy_dat);

    static void finalizeSimulation(double duracion, std::ofstream& csv);

    static void configureExternalSource(Network& myNetwork, int num_nodes);
};

#endif