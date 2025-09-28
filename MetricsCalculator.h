#include <vector>

class MetricsCalculator{
public:
    static double CalcularEnergia(const std::vector<double>& A);
    static double CalcularPromedio(const std::vector<double>& A);
private:
    double tiempo;
    double energia;
    double amplitudPromedio;

};