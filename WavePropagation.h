#include "Network.h"

class WavePropagator {
    private:
        Network *network;
        double time_step;
        std::vector<double> sources;
        double energy;

    public:

        //Constructor
        WavePropagator(Network *net, double dt, std::vector<double> src, double initial_energy);

        //Getter
        double GetEnergy();

        // Function overloading para integracion con diferentes clausulas
        void integrateEuler(); // Integracion basica
        void integrateEuler(int sync_type); // atomic=0, critical=1, nowait=2
        void integrateEuler(int sync_type, bool use_barrier);

        // Function overloading para calculo de energia
        void calculateEnergy(); // Calculo basico
        void calculateEnergy(int method); // reduce=0, atomic=1
        void calculateEnergy(int method, bool use_private);

        // Function overloading para procesamiento de nodos
        void processNodes(); // Procesamiento basico
        void processNodes(int task_type); // task=0, parallel_for=1
        void processNodes(int task_type, bool use_single);

        // Metodos especificos para clausulas unicas
        void simulatePhasesBarrier(); // Con barrier
        void parallelInitializationSingle(); // Con single
        void calculateFinalStateLastprivate();
        void calculateMetricsFirstprivate();
};