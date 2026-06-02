#pragma once
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "DateRange.hpp"
#include "portfolio.hpp"

namespace teamA {
    typedef std::vector<std::pair<std::string, double> > TimeSeries;

    TimeSeries FilterPrices(const TimeSeries data, const std::vector<std::string> &range);

    TimeSeries LogReturns(const TimeSeries prices);

    inline void printProgressBar(float progress, int width = 40) {
        int pos = static_cast<int>(width * progress);
        // Carriage return to overwrite the current line
        std::cout << "\r[";
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        // Print percentage with fixed width and a single '%'
        std::cout << "] "
                << std::setw(3) << static_cast<int>(progress * 100.0f)
                << " %"
                << std::flush;
    }

    template<typename Func>
    void runWithProgressBar(Func task) {
        // inject our printer
        auto progressCallback = [](float p) {
            printProgressBar(p);
        };

        task(progressCallback);
        std::cout << std::endl << "Data fetched" << std::endl;
    }

    std::ostream &operator<<(std::ostream &os, const TimeSeries &ts);

    std::ostream &operator<<(std::ostream &os, const std::vector<double> &vec);
}
