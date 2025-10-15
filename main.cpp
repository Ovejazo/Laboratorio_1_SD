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

#include <omp.h>

/* tipo de ejecuciones

- 0 para static
- 1 dynamic
- 2 guided

ejemplo:
- ./wave_propagation 0
- ./wave_propagation 1 4
- ./wave_propagation 2 8
*/

// Escribe scaling analysis.dat tomando, para cada p, el mejor tiempo (mínimo) entre todas las combinaciones.

//------------------------------ MAIN --------------------------------------
int main(int argc, char** argv) {
    if (argc >= 2 && std::string(argv[1]) == "-benchmark"){
        std::vector<int> schedules = {0, 1, 2};      // static, dynamic, guided
        std::vector<int> chunks    = {0, 64, 256};   // 0 => sin chunk explícito
        std::vector<int> threads   = {1, 2, 4, 8};

        //Creamos el directorio donde se guardara la información
        std::filesystem::create_directories("datos");

        std::vector<double> t1_samples;
        for(int r = 0; r < 10; ++r){
            t1_samples.push_back(Benchmark::run_once_benchmark(0, 0, 1));
        }

        double m = 0.0;
        for(double x : t1_samples) m += x;
        m /= t1_samples.size();
        double s2 = 0.0;
        for (double x : t1_samples){
            double d = x - m;
            s2 += d*d;
        }
        double s = (t1_samples.size() > 1) ? std::sqrt(s2/(t1_samples.size()-1)) : 0.0;

        auto results = Benchmark::runGrid(
            schedules, chunks, threads,
            10,
            Benchmark::run_once_benchmark,
            m, s);

        Benchmark::writeDat("datos/benchmark results.dat", results);
        Benchmark::writeScalingAnalysis(results, m, s, "datos/scaling analysis.dat");
        std::cout << "Resultados de benchmark escritos";
        return 0;
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

    //Fuente externa
    std::vector<double> sources(num_nodes, 0.0);
    
    std::cout << "Parametros: \n- Nodos=" << num_nodes << ", \n- D=" << D << ", \n- gamma=" << gamma;
    std::cout << ", \n- dt=" << dt << ", \n- Pasos=" << num_steps << std::endl;

    //Se crea una red que se llamara "my_network"
    Network myNetwork(num_nodes, D, gamma);

    //Creamos un red. 1 para 1D y 2 para 2D
    myNetwork.initializeRegularNetwork(2, 10, 10);
    myNetwork.setTimeStep(dt);
    //myNetwork.setSources(sources);

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

    //Vamos a definir la pertubación inicial para que la señal se mueva
    myNetwork.getNode(num_nodes/2).setAmplitude(1.0);

    //Creamos el objeto WavePropagator
    WavePropagator propagation(&myNetwork, dt, sources, energy);

    //Creamos el directorio para guardar los datos en el mismo lugar
    std::filesystem::create_directories("datos");

    // 1. ABRIR ARCHIVO CSV PARA ESCRITURA (EVOLUCIÓN COMPLETA)
    std::ofstream csv("results.csv");
    if (!csv.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo results.csv" << std::endl;
        return 1;
    }
    std::ofstream wave_dat("datos/wave evolution.dat");
    std::ofstream energy_dat("datos/energy conservation.dat");
    if(!wave_dat.is_open() || !energy_dat.is_open()){
        std::cerr << "Error: No se pudo abrir wave evolution o energy conservation";
        return 1;
    }

// 2. ESCRIBIR CABECERA (Time_Step + Node_0, Node_1, ..., Node_N-1)
    csv << "Time_Step,energy,avg_amp";
    wave_dat << "# Time_Step";
    for (int i = 0; i < num_nodes; ++i) {
        csv << ",Node_" << i;
        wave_dat << " Node_" << i; // <-- espacio antes de Node_
    }
    csv << "\n";
    wave_dat << "\n";
    energy_dat << "# Time_Step energy\n";

    // 3. ESTADO INICIAL
    propagation.calculateEnergy(0);
    std::vector<double> initial_amplitudes = myNetwork.getCurrentAmplitudes();
    double avg0 = 0.0;
    for (double v : initial_amplitudes) avg0 += v;
    avg0 /= initial_amplitudes.size();

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

    // 4. BUCLE PRINCIPAL
    double t0 = omp_get_wtime();
    for (int step = 1; step <= num_steps; ++step) {

        if(use_collapse){
            
            //Va a entrar aquí unicamente si la red es 2D
            myNetwork.propagateWavesCollapse();
        }else{
            if (chunk_size > 0) myNetwork.propagateWaves(schedule_type, chunk_size);
                else myNetwork.propagateWaves(schedule_type);
        }

        propagation.calculateEnergy(1); // reduction

        std::vector<double> current_amplitudes = myNetwork.getCurrentAmplitudes();

        // promedio de amplitudes
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

    // 5. CERRAR ARCHIVO
    double t1 = omp_get_wtime();
    std::cout << "Tiempo total: " << (t1 - t0) << "s\n";
    csv.close();
    std::cout << "Datos de evolucion guardados en 'results.csv'." << std::endl;
    
    propagation.parallelInitializationSingle();
    propagation.calculateMetricsFirstprivate();
    propagation.calculateFinalStateLastprivate();
    propagation.simulatePhasesBarrier();

    return 0;




/*
        schedule_type: 0=static, 1=dynamic, 2=guided
        propagation.integrateEuler(2, true);
*/

        //Ahora vamos a implementar propagateWaves()
/*
        propagation.calculateEnergy(1);
        propagation.processNodes(0, false);
 */
        // Escribir resultados de este paso en el CSV

        // Imprimir en consola cada 25 pasos


}