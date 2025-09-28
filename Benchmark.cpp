#include "Benchmark.h"

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
    out.reserve(schedules.size() * chunk.size() * threadsList.size());

    //Vamos a hacer un for por cada "item" que tengamos
    for(int p : threadsList){
        for (int sch : schedules){
            for (int ch : chunk){
                std::vector<double> times;
                time.reserve(repetitions);
                for(int r = 0; r < repetitions; ++r){
                    time.push_back(runFn(sch, ch, p));
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
                    Estadisticas(sigma_Sp, 0.0)
                    sigma_Ep
                );
            }
        }
    }
    return out;
}

void Benchmark::writeDat(const std::string& path, const std::vector<RunResult>& rows) {
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

//Getters
int getThreads() {return threads;}
int getSchedule() {return schedule;} 
int getchunk() {return chunk;}
double getSpeedup() {return speedup;}
double getEfficiency() {return efficiency;}
double getEfficiencyErr() {return efficiencyErr;}
const Estadisticas& getTime() {return time;}
const Estadisticas& getSpeedupErr() {return speedupErr;}

