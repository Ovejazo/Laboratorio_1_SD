#include "Benchmark.h"
#include "Network.h"
#include <cmath>
#include <fstream>
#include <omp.h>

static Estadisticas computeMeanStd(const std::vector<double>& v){
    if(v.empty()) return Estadisticas(0.0, 0.0);

    double m = 0.0;
    for(double x : v) m += x;
    m /= static_cast<double>(v.size());
    double s2 = 0.0;
    if(v.size() > 1){
        for(double x : v){
            double d = x - m;
            s2 +=  d*d;
        }
        s2 /= static_cast<double>(v.size() - 1);
    }
    return Estadisticas(m, std::sqrt(s2));
}

double Benchmark::run_once_benchmark(int schedule, int chunk, int threads){
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

std::vector<RunResults> Benchmark::runGrid(
    const std::vector<int>& schedules,
    const std::vector<int>& chunks,
    const std::vector<int>& threadsList,
    int repetitions,
    const std::function<double(int,int,int)>& runFn,
    double t1_mean, double t1_std)
{

    //Declaramos la salida, que sera un vector de tipo "RunResults"
    std::vector<RunResults> out;
    out.reserve(schedules.size() * chunks.size() * threadsList.size());

    //Vamos a hacer un for por cada "item" que tengamos
    for(int p : threadsList){
        for (int sch : schedules){
            for (int ch : chunks){
                std::vector<double> times;
                times.reserve(repetitions);
                for(int r = 0; r < repetitions; ++r){
                    times.push_back(runFn(sch, ch, p));
                }
                Estadisticas t = computeMeanStd(times);

                const double Sp = (t1_mean > 0.0) ? (t1_mean / t.getMedia()) : 0.0;
                const double Ep = (p > 0) ? (Sp / p) : 0.0;

                const double rel_T1 = (t1_mean > 0.0) ? (t1_std /t1_mean) : 0.0;
                const double rel_Tp = (t.getMedia() > 0.0) ? (t.getStddev() / t.getMedia()) : 0.0;
                const double sigma_Sp = Sp * std::sqrt(rel_T1*rel_T1 + rel_Tp*rel_Tp);
                const double sigma_Ep = (p > 0) ? (sigma_Sp / p) : 0.0;

                out.emplace_back(
                    p, sch, ch,
                    t, Sp, Ep,
                    Estadisticas(sigma_Sp, 0.0),
                    sigma_Ep);
            }
        }
    }
    return out;
}

void Benchmark::writeDat(const std::string& path, const std::vector<RunResults>& rows) {
    std::ofstream f(path);
    f << "#threads schedule chunk time_mean time_std speedup efficiency sigma_Sp sigma_Ep\n";
    for (const auto& r : rows) {
        f << r.getThreads() << " "
          << r.getSchedule() << " "
          << r.getChunk() << " "
          << r.getTime().getMedia() << " "
          << r.getTime().getStddev() << " "
          << r.getSpeedup() << " "
          << r.getEfficiency() << " "
          << r.getSpeedupErr().getMedia() << " "
          << r.getEfficiencyErr() << "\n";
    }
}

void Benchmark::writeScalingAnalysis(const std::vector<RunResults>& rows,
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