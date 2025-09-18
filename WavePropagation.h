#include "Network.h"

class WavePropagator {
    private:
        Network* network;
        double time_step;
    public:
        WavePropagator(Network* net, double dt);
        // Function overloading para integraci´on con diferentes cl´ausulas
        void integrateEuler(); // Integraci´on b´asica
        void integrateEuler(int sync_type); // atomic=0, critical=1, nowait=2
        void integrateEuler(int sync_type, bool use_barrier);

        // Function overloading para c´alculo de energ´ıa
        void calculateEnergy(); // C´alculo b´asico
        void calculateEnergy(int method); // reduce=0, atomic=1
        void calculateEnergy(int method, bool use_private);
        // Function overloading para procesamiento de nodos
        void processNodes(); // Procesamiento b´asico
        void processNodes(int task_type); // task=0, parallel_for=1
        void processNodes(int task_type, bool use_single);
        // M´etodos espec´ıficos para cl´ausulas ´unicas
        void simulatePhasesBarrier(); // Con barrier
        void parallelInitializationSingle(); // Con single
};