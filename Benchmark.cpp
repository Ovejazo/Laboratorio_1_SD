    #include "Benchmark.h"
    

    //Getters
    int getThreads() {return threads;}
    int getSchedule() {return schedule;} 
    int getchunk() {return chunk;}
    double getSpeedup() {return speedup;}
    double getEfficiency() {return efficiency;}
    double getEfficiencyErr() {return efficiencyErr;}
    const Estadisticas& getTime() {return time;}
    const Estadisticas& getSpeedupErr() {return speedupErr;}