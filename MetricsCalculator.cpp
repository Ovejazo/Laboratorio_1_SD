#include "MetricsCalculator.h"
#include <numeric>
#include <cmath>

/*
metodo: CalcularEnergia
descripcion: Calcula la energia total del sistema
retorno: double que representa la energia
*/
double MetricsCalculator::CalcularEnergia(const std::vector<double>& A){
    double e = 0.0;
    for(double x : A) e += x * x;
    return e;
}

/*
metodo: CalcularPromedio
descripcion: Obtien el promedio de las amplitudes
retorno: double que representa el promedio
*/
double MetricsCalculator::CalcularPromedio(const std::vector<double>& A){
    if(A.empty()) return 0.0;
    double s = std::accumulate(A.begin(), A.end(), 0.0);
    return s / static_cast<double>(A.size());
}