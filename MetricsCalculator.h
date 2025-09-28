#include <vector>

class MetricsCalculator{
public: 
    double tiempo;
    double energia;
    double amplitudPromedio;
private:
    static double CalcularEnergia(const std::vector<double>& A);
    static double CalcularPromedio(const std::vector<double>& A);
};