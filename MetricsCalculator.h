#include <vector>

/*
Abstracción:
Clase creada para calcular el comportaaaamiento de la energia a través del tiempo y/o tipo de malla, nodos, entre otros
*/

class MetricsCalculator{
public:
    //otros metodos
    static double CalcularEnergia(const std::vector<double>& A);
    static double CalcularPromedio(const std::vector<double>& A);
private:
    //datos privados
    double tiempo;
    double energia;
    double amplitudPromedio;

};