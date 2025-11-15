#include "Network.h"

/*
Abstracción:
Clase encargadad de propagar la energia a traves de formulas
*/

class WavePropagator {
    private:
        //datos privados
        Network *network;
        double time_step;
        std::vector<double> sources;
        double energy;

    public:

        //Constructor
        WavePropagator(Network *net, double dt, std::vector<double> src, double initial_energy);

        //Getter
        double GetEnergy();

        //Otros metodos 
        // Funcion overloading para calculo de energia
        void calculateEnergy(); // Calculo basico
        void calculateEnergy(int method); // reduce=0, atomic=1
        void calculateEnergy(int method, bool use_private);

        // Funcion overloading para procesamiento de nodos
        void processNodes(); // Procesamiento basico
        void processNodes(int task_type); // task=0, parallel_for=1
        void processNodes(int task_type, bool use_single);

        // Metodos especificos para clausulas especificasss
        void simulatePhasesBarrier(); // Con barrier
        void parallelInitializationSingle(); // Con single
        void calculateFinalStateLastprivate();
        void calculateMetricsFirstprivate();
};