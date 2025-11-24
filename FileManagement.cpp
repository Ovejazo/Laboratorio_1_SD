
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <omp.h>

#include "Network.h"
#include "WavePropagation.h"
#include "FileManagement.h"

/*
metodo: crearCarpeta
descripcion: Crea la carpeta "datos" si no existe
retorno: -
*/
void FileManagement::crearCarpeta() {
    std::filesystem::create_directories("datos");
}

/*
metodo: openOutFiles
descripcion: Abre los archivos de salida para resultados, evolución de ondas y conservación de energía
retorno: -
*/
bool FileManagement::openOutFiles(std::ofstream& csv, std::ofstream& wave_dat, std::ofstream& energy_dat){
    csv.open("results.csv");
    wave_dat.open("datos/wave evolution.dat");
    energy_dat.open("datos/energy conservation.dat");

    if(!wave_dat.is_open() || !energy_dat.is_open() || !csv.is_open()){
        std::cerr << "Error: No se pudo abrir wave evolution o energy conservation";
        return 1;
    }
    return true;
}

/*
metodo: writeHeader
descripcion: Escribe los encabezados en los archivos de salida para resultados,
             evolución de ondas y conservación de energía
retorno: -
*/
void FileManagement::writeHeader(std::ofstream& csv,
                         std::ofstream& wave_dat,
                         std::ofstream& energy_dat,
                         int num_nodes){
    csv << "Time_Step,energy,avg_amp";
    wave_dat << "# Time_Step";
    for (int i = 0; i < num_nodes; ++i) {
        csv << ",Node_" << i;
        wave_dat << " Node_" << i; 
    }
    csv << "\n";
    wave_dat << "\n";
    energy_dat << "# Time_Step energy\n";
}

/*
metodo: writeInitialState
descripcion: Escribe el estado inicial de la simulación en los archivos de salida
retorno: -
*/
void FileManagement::writeInitialState(Network& myNetwork,
                              WavePropagator& propagation,
                              std::ofstream& csv,
                              std::ofstream& wave_dat,
                              std::ofstream& energy_dat){
    propagation.calculateEnergy(0);
    std::vector<double> initial_amplitudes = myNetwork.getCurrentAmplitudes();

    double avg0 = 0.0;
    for (double v : initial_amplitudes) avg0 += v;
    if(!initial_amplitudes.empty()) avg0 /= static_cast<double>(initial_amplitudes.size());

    csv << 0 << "," << std::scientific << std::setprecision(6) << propagation.GetEnergy()
        << "," << std::scientific << std::setprecision(6) << avg0;
    wave_dat << 0;
    for (double amp : initial_amplitudes) {
        csv << "," << std::scientific << std::setprecision(6) << amp;
        wave_dat << " " << std::scientific << std::setprecision(6) << amp;
    }
    csv << "\n";
    wave_dat << "\n";
    energy_dat << 0 << " " << std::scientific << std::setprecision(6) << propagation.GetEnergy() << "\n";        
}

/*
metodo: finalizeSimulation
descripcion: Finaliza la simulación cerrando el archivo CSV y mostrando el tiempo total
             de ejecución en la consola
retorno: -
*/
void FileManagement::finalizeSimulation(double duracion, std::ofstream& csv){
    std::cout << "Tiempo total: " << duracion << "s\n";
    csv.close();
    std::cout << "Datos de evolucion guardados en 'results.csv'." << std::endl;
}

/*
metodo: configureExternalSource
descripcion: Estaba configurando la fuente externa en la red, aquí se tiene varias opciones predefinidas, zero, ajustado, random y senoidal.
retorno: -
*/
void FileManagement::configureExternalSource(Network& myNetwork, int num_nodes) {

    enum class SourcePreset { Zero, Fixed, Random, Sine };

    //constexpr SourcePreset kSource = SourcePreset::Zero;
    constexpr SourcePreset kSource = SourcePreset::Fixed;
    //constexpr SourcePreset kSource = SourcePreset::Random;
    //constexpr SourcePreset kSource = SourcePreset::Sine;

    constexpr double kFixedValue = 0.05;           // Fixed
    constexpr double kRandMin = -0.05;             // Random
    constexpr double kRandMax =  0.05;
    constexpr unsigned kSeed   = 1234;
    constexpr double kAmp   = 0.1;                 // Sine
    constexpr double kOmega = 2.0 * M_PI * 1.0;    // ω = 2πf (f=1 Hz)

    // Aplica la configuración elegida
    switch (kSource) {
        case SourcePreset::Zero:
            myNetwork.setZeroSource();
            break;
        case SourcePreset::Fixed: {
            std::vector<double> s(num_nodes, kFixedValue);
            myNetwork.setSources(s);
            break;
        }
        case SourcePreset::Random:
            myNetwork.generateRandomSources(kRandMin, kRandMax, kSeed);
            break;
        case SourcePreset::Sine:
            myNetwork.setSineSource(kAmp, kOmega);
            break;
    }
}