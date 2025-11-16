#include <random>
#include <string>
#include <vector>
#include <functional>

/*
Abstracción:
Esta clase corresponden a las clases necesarias para utilizar Benchmark
*/

class Estadisticas{
public:
    //constructores
    Estadisticas() : media(0.0), stddev(0.0) {}
    Estadisticas(double media, double stddev) : media(media), stddev(stddev) {}

    //otros metodos
    double getMedia() const { return media; }
    double getStddev() const { return stddev; }
    void setMedia(double v) { media = v; }
    void setStddev(double v) { stddev = v; }

    
private:
    //datos privados
    double media;
    double stddev;
};

class RunResults{
public:
    //constructores
    RunResults() :
        threads(1), schedule(0), chunk(0),
        time(), speedup(1.0), efficiency(1.0),
        speedupErr(), efficiencyErr(0.0) {}


    RunResults(int threads, int schedule, int chunk,
              const Estadisticas& time, double speedup, double efficiency,
              const Estadisticas& speedupErr, double efficiencyErr)
        : threads(threads), schedule(schedule), chunk(chunk),
          time(time), speedup(speedup), efficiency(efficiency),
          speedupErr(speedupErr), efficiencyErr(efficiencyErr) {}
    
    // otros metodos - getters
    int getThreads() const { return threads; }
    int getSchedule() const { return schedule; }
    int getChunk() const { return chunk; }
    double getSpeedup() const { return speedup; }
    double getEfficiency() const { return efficiency; }
    double getEfficiencyErr() const { return efficiencyErr; }
    const Estadisticas& getTime() const { return time; }
    const Estadisticas& getSpeedupErr() const { return speedupErr; }


private:
    //datos privados
    int threads;
    int schedule;
    int chunk;
    Estadisticas time;
    double speedup;
    double efficiency;
    Estadisticas speedupErr;
    double efficiencyErr;
};

class Benchmark{
public:
    //Otros metodos
    static std::vector<RunResults> runGrid(
        const std::vector<int>& schedules,
        const std::vector<int>& chunks,
        const std::vector<int>& threadsList,
        int repetitions,
        const std::function<double(int, int, int)>& runFn,
        double t1_promedio, double t1_std);
    
    static void writeDat(const std::string& path, const std::vector<RunResults>& rows);

    static double run_once_benchmark(int schedule, int chunk, int threads);

    static void writeScalingAnalysis(const std::vector<RunResults>& rows,
                                    double t1_mean, double t1_std,
                                    const std::string& path);

    static int runBenchmark();
};