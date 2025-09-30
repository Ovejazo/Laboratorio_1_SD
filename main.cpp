#include <ios>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>

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
//------------------------------ BENCHMARK --------------------------------------
static double run_once_benchmark(int schedule, int chunk, int threads){
    omp_set_num_threads(threads);

    //DEfinimos los parametros
    const int num_nodes = 5000;
    const double D = 0.1;
    const double gamma = 0.01;
    const double dt  = 0.01;
    const int num_steps = 200;

    std::vector<double> sources (num_nodes, 0.0);

    Network net(num_nodes, D, gamma);
    net.initializeRegularNetwork(1);
    net.setTimeStep(dt);
    net.setSources(sources);
    net.getNode(num_nodes/2).setAmplitude(1.0);

    double t0 = omp_get_wtime();
    for (int step = 0; step < num_steps; ++step){
        if(chunk > 0)   net.propagateWaves(schedule, chunk);
        else            net.propagateWaves(schedule);  
    }
    double t1 = omp_get_wtime();
    return (t1 - t0);
}

// Escribe scaling analysis.dat tomando, para cada p, el mejor tiempo (mínimo) entre todas las combinaciones.
static void writeScalingAnalysis(const std::vector<RunResults>& rows,
                                 double t1_mean, double t1_std,
                                 const std::string& path) {
    // Agrupa por threads y selecciona la fila con menor time_mean
    std::ofstream f(path);
    f << "#threads time_mean time_std speedup efficiency sigma_Sp sigma_Ep schedule chunk\n";

    // Recolectar conjunto de threads
    std::vector<int> all_threads;
    all_threads.reserve(rows.size());
    for (const auto& r : rows) all_threads.push_back(r.getThreads());
    std::sort(all_threads.begin(), all_threads.end());
    all_threads.erase(std::unique(all_threads.begin(), all_threads.end()), all_threads.end());

    for (int p : all_threads) {
        const RunResults* best = nullptr;
        for (const auto& r : rows) {
            if (r.getThreads() != p) continue;
            if (!best || r.getTime().getMedia() < best->getTime().getMedia()) best = &r;
        }
        if (!best) continue;

        const double Tp_mean = best->getTime().getMedia();
        const double Tp_std  = best->getTime().getStddev();

        const double Sp = (Tp_mean > 0.0) ? (t1_mean / Tp_mean) : 0.0;
        const double Ep = (p > 0) ? (Sp / p) : 0.0;

        const double rel_T1 = (t1_mean > 0.0) ? (t1_std / t1_mean) : 0.0;
        const double rel_Tp = (Tp_mean > 0.0) ? (Tp_std / Tp_mean) : 0.0;
        const double sigma_Sp = Sp * std::sqrt(rel_T1*rel_T1 + rel_Tp*rel_Tp);
        const double sigma_Ep = (p > 0) ? (sigma_Sp / p) : 0.0;

        f << p << " "
          << Tp_mean << " " << Tp_std << " "
          << Sp << " " << Ep << " "
          << sigma_Sp << " " << sigma_Ep << " "
          << best->getSchedule() << " " << best->getChunk() << "\n";
    }
}


//------------------------------ MAIN --------------------------------------
int main(int argc, char** argv) {
    if (argc >= 2 && std::string(argv[1]) == "-benchmark"){
        std::vector<int> schedules = {0, 1, 2};      // static, dynamic, guided
        std::vector<int> chunks    = {0, 64, 256};   // 0 => sin chunk explícito
        std::vector<int> threads   = {1, 2, 4, 8};

        std::vector<double> t1_samples;
        for(int r = 0; r < 10; ++r){
            t1_samples.push_back(run_once_benchmark(0, 0, 1));
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
            run_once_benchmark,
            m, s);

        Benchmark::writeDat("benchmark results.dat", results);
        writeScalingAnalysis(results, m, s, "scaling analysis.dat");
        std::cout << "Resultados de benchmark escritos";
        return 0;
    }

    //Vamos a definir el schedule_type y el chunk_size como valores de entrada
    int schedule_type = 0;
    int chunk_size = 0;

    //Conseguimos valores dependiendo del valor de argc
    if(argc >= 2) schedule_type = std::stoi(argv[1]);
    if(argc >= 3) chunk_size = std::stoi(argv[2]);

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

    //Creamos un red. 0 para 1D y 1 para 2D
    myNetwork.initializeRegularNetwork(1);
    myNetwork.setTimeStep(dt);
    myNetwork.setSources(sources);

    //Vamos a definir la pertubación inicial para que la señal se mueva
    myNetwork.getNode(num_nodes/2).setAmplitude(1.0);

    //Creamos el objeto WavePropagator
    WavePropagator propagation(&myNetwork, dt, sources, energy);

    // 1. ABRIR ARCHIVO CSV PARA ESCRITURA (EVOLUCIÓN COMPLETA)
    std::ofstream csv("results.csv");
    if (!csv.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo results.csv" << std::endl;
        return 1;
    }
    std::ofstream wave_dat("wave evolution.dat");
    std::ofstream energy_dat("energy conservation.dat");
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
        if (chunk_size > 0) {
            myNetwork.propagateWaves(schedule_type, chunk_size);
        } else {
            myNetwork.propagateWaves(schedule_type);
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