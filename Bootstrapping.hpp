#pragma once

#include <vector>
#include <string>
#include <list>
#include <cmath>
#include <string>
#include <algorithm>
#include <random> 
#include "portfolio.hpp"

using namespace teamA;

namespace teamA {
 class Bootstrap {
    protected: 
        std::vector<std::vector<double>> simARR;
        std::vector<std::vector<double>> simCAAR;
        std::vector<double> AvgARR;
        std::vector<double> AvgCAAR;
        std::vector<double> ARRstd;
        std::vector<double> CAARstd;
        double FinalAvgARR;
        double FinalAvgCAAR;
        double FinalStdARR;
        double FinalStdCAAR;
    public:    
    Bootstrap() {};
    void BootstrapSim(const Portfolio& portfolio,int sample_, int sim_, int N_);
    void setAvgs();
    Portfolio sampleGroup(const Portfolio& portfolio, int sample_);
    std::vector<double> getAvgARR() const { return AvgARR; }
    std::vector<double> getAvgCAAR() const { return AvgCAAR; }
    std::vector<std::vector<double>> getSimARR() const { return simARR; }
    std::vector<std::vector<double>> getSimCAAR() const { return simCAAR; }
    double getFinalAvgARR() const { return FinalAvgARR; }
    double getFinalAvgCAAR() const { return FinalAvgCAAR; }
    double getFinalStdARR() const {return FinalStdARR;};
    double getFinalStdCAAR() const {return FinalStdCAAR;};
    void setSTD();
    std::vector<double> getARRstd() const { return ARRstd; }
    std::vector<double> getCAARstd() const { return CAARstd;}
};
}
