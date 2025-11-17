#include <ios>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <filesystem>

#include "WavePropagation.h"
#include "Benchmark.h"
#include "MetricsCalculator.h"
#include "FileManagement.h"

#include <omp.h>

int main(int argc, char** argv) {
    if (argc >= 2 && std::string(argv[1]) == "-benchmark"){
        return Benchmark::runBenchmark();
    }

    //Vamos a definir el schedule_type y el chunk_size como valores de entrada
    int schedule_type = 0;
    int chunk_size = 0;
    bool use_collapse = false;

    //Conseguimos valores dependiendo del valor de argc
    if(argc >= 2) schedule_type = std::stoi(argv[1]);
    if(argc >= 3) chunk_size = std::stoi(argv[2]);

    //Si se quiere hacer un red 2D se puede agregar la flag -collapse
    if(argc >= 4 && std::string(argv[3]) == "-collapse") use_collapse = true;

    //Inicializamos los parametros con los que vamos a trabajar
    const int num_nodes = 100;
    const double D = 6;
    const double gamma = 0.01;
    const double dt = 0.01;
    const int num_steps = 1000;
    double energy = 0.0;
    
    std::cout << "Parametros: \n- Nodos=" << num_nodes << ", \n- D=" << D << ", \n- gamma=" << gamma;
    std::cout << ", \n- dt=" << dt << ", \n- Pasos=" << num_steps << std::endl;

    //Se crea una red que se llamara "my_network"
    Network myNetwork(num_nodes, D, gamma);

    //Creamos un red. 1 para 1D y 2 para 2D
    myNetwork.initializeRegularNetwork(1);
    myNetwork.setTimeStep(dt);

    FileManagement::configureExternalSource(myNetwork, num_nodes);

    //Vamos a definir la pertubación inicial para que la señal se mueva
    myNetwork.getNode(num_nodes/2).setAmplitude(1.0);

    //Creamos el objeto WavePropagator
    std::vector<double> dummy_sources;
    WavePropagator propagation(&myNetwork, dt, dummy_sources, energy);

    
    //1. Creamos las carpetas necesarias y abrimos los archivos de salida
    FileManagement::crearCarpeta();
    std::ofstream csv, wave_dat, energy_dat;
    if(!FileManagement::openOutFiles(csv, wave_dat, energy_dat)){
        return 1;
    }
    
    //2. Escribimos la cabecera de los archivos
    FileManagement::writeHeader(csv, wave_dat, energy_dat, num_nodes);

    //3. Se escriben los estados iniciales
    FileManagement::writeInitialState(myNetwork, propagation, csv, wave_dat, energy_dat);

    //4. Loop principal de la simulación
    double t0 = omp_get_wtime();
    for (int step = 1; step <= num_steps; ++step) {

        if(use_collapse){
            myNetwork.propagateWavesCollapse();// En caso de que sea 2D

        }else{
            if (chunk_size > 0) myNetwork.propagateWaves(schedule_type, chunk_size);
            else                myNetwork.propagateWaves(schedule_type);
        }

        propagation.calculateEnergy(1); // reduction

        std::vector<double> current_amplitudes = myNetwork.getCurrentAmplitudes();

        // Se consigue el promedio de la amplitudes
        double avg = 0.0;
        for (double v : current_amplitudes) avg += v;
        avg /= current_amplitudes.size();

        // Escribir CSV + DAT (ondas y energía)
        csv << step << "," << std::scientific << std::setprecision(6) << propagation.GetEnergy()
            << "," << std::scientific << std::setprecision(6) << avg;
        wave_dat << step;
        
        for (double amp : current_amplitudes) {
            csv << "," << std::scientific << std::setprecision(6) << amp;
            wave_dat << " " << std::scientific << std::setprecision(6) << amp;
        }

        csv << "\n";
        wave_dat << "\n";
        energy_dat << step << " " << std::scientific << std::setprecision(6) << propagation.GetEnergy() << "\n";
    }

    //5. Cerramos los archivos y finalizamos la simulación
    double t1 = omp_get_wtime();
    const double duracion = t1 - t0;

    FileManagement::finalizeSimulation(duracion, csv);
    
    //Funciones de prueba de clausulas OpenMP
    propagation.parallelInitializationSingle();
    propagation.calculateMetricsFirstprivate();
    propagation.calculateFinalStateLastprivate();
    propagation.simulatePhasesBarrier();

    return 0;
}