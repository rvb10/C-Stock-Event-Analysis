#include "Bootstrapping.hpp"
#include "portfolio.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <random>

using namespace teamA;

using namespace teamA;

Portfolio Bootstrap::sampleGroup(const Portfolio &portfolio, int sample_) {
    std::vector<Stock *> stocks = portfolio.getStocks();
    std::vector<Stock *> sample_stocks;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(stocks.begin(), stocks.end(), gen);
    for (size_t i = 0; i <= sample_; ++i) {
        sample_stocks.push_back(stocks[i]);
    }
    Portfolio sample_portfolio;
    sample_portfolio.setStocks(sample_stocks);
    return sample_portfolio;
}

void Bootstrap::BootstrapSim(const Portfolio& portfolio, int sample_, int sim_, int N_) {
    simARR.clear();
    simCAAR.clear();
    for (int i = 0; i < sim_; i++) {
        Portfolio sample_portfolio = sampleGroup(portfolio, sample_);
        sample_portfolio.setExpectedSize(2*N_+1);
        sample_portfolio.setAAR();
        sample_portfolio.setCAAR();
        std::vector<double> sample_ARR = sample_portfolio.getARR();
        std::vector<double> sample_CAAR = sample_portfolio.getCAAR();
        simARR.push_back(sample_ARR);
        simCAAR.push_back(sample_CAAR);
    }
}

void Bootstrap::setAvgs() {
    // Vector (2N+1) of average ARR and CAAR
    AvgARR.clear();
    AvgCAAR.clear();
    AvgARR.resize(simARR[0].size(), 0);
    AvgCAAR.resize(simCAAR[0].size(), 0);
    for (int i = 0; i < simARR[0].size(); i++) { //For each time period
        for (int j = 0; j < simARR.size(); j++) { //For each simulation
            AvgARR[i] += simARR[j][i];
            AvgCAAR[i] += simCAAR[j][i];
        }
        AvgARR[i] /= simARR.size();
        AvgCAAR[i] /= simCAAR.size();
    }
    // Double final averages:
    FinalAvgARR = 0.0;
    FinalAvgCAAR = 0.0;
    for (int i = 0; i < AvgARR.size(); i++) {
        FinalAvgARR += AvgARR[i];
        FinalAvgCAAR += AvgCAAR[i];
    }
    FinalAvgARR /= AvgARR.size();
    FinalAvgCAAR /= AvgCAAR.size();
}

void Bootstrap::setSTD() {
    ARRstd.clear();
    CAARstd.clear();
    ARRstd.resize(simARR[0].size(), 0);
    CAARstd.resize(simCAAR[0].size(), 0);

    for (int i = 0; i < simARR[0].size(); i++) { //For each time period
        double mean_a = 0.0, mean_c = 0.0;
        for (int j = 0; j < simARR.size(); j++) { //For each simulation
            mean_a += simARR[j][i];
            mean_c += simCAAR[j][i];
        }
        mean_a /= simARR.size(); 
        mean_c /= simCAAR.size();
        double variance_a = 0.0, variance_c = 0.0;
            for (int j = 0; j < simARR.size(); j++) {
                variance_a += (simARR[j][i] - mean_a) * (simARR[j][i] - mean_a);
                variance_c += (simCAAR[j][i] - mean_c) * (simCAAR[j][i] - mean_c);
            }
        ARRstd[i] = std::sqrt(variance_a / (simARR.size() * simARR.size()));
        CAARstd[i] = std::sqrt(variance_c / (simCAAR.size() * simCAAR.size()));  
        
        //Final stds: 
        double mean_1 = 0.0;
        for (double value : AvgARR) {
            mean_1 += value;
        }
        mean_1 /= AvgARR.size();

        double variance_1 = 0.0;
        for (double value : AvgARR) {
            variance_1 += (value - mean_1) * (value - mean_1);
        }
        variance_1 /= AvgARR.size();

        FinalStdARR = std::sqrt(variance_1);
        
        double mean_2 = 0.0;
        for (double value : AvgCAAR) {
            mean_2 += value;
        }
        mean_2 /= AvgCAAR.size();

        double variance_2 = 0.0;
        for (double value : AvgCAAR) {
            variance_2 += (value - mean_2) * (value - mean_2);
        }
        variance_2 /= AvgCAAR.size();
        FinalStdCAAR = std::sqrt(variance_2);
    }
}
