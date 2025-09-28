#include "MetricsCalculator.h"
#include <numeric>
#include <cmath>

double MetricsCalculator::CalcularEnergia(const std::vector<double>& A){
    double e = 0.0;
    for(double x : A) e += x * x;
    return e;
}

double MetricsCalculator::CalcularPromedio(const std::vector<double>& A){
    if(A.empty()) return 0.0;
    double s = std::accumulate(A.begin(), A.end(), 0.0);
    return s / static_cast<double>(A.size());
}