#include <string>
#include <vector>
#include <functional>

class Estadisticas{
public:
    Estadisticas() : media(0.0), stddev(0.0) {}

    double getMedia();
    double getStddev();

    void setMedia(double v);
    void setStddev(double v);

private:
    double media;
    double stddev;
};

class RunResults{
public:
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
    
    int getThreads();
    int getSchedule();
    int getchunk();
    double getSpeedup();
    double getEfficiency();
    double getEfficiencyErr();
    const Estadisticas& getTime();
    const Estadisticas& getSpeedupErr();

private:
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
    static std::vector<RunResults> runGri(
        const std::vector<int>& schedule,
        const std::vector<int>& schedules,
        const std::vector<int>& chunks,
        const std::vector<int>& threadsList,
        int repetitions,
        const std::function<double(int, int, int)>& runFn,
        double t1_promedio, double t1_std);
    
    static void writeDat(const std::string& path, const std::vector<RunResults>& rows);
};