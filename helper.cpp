#include "DateRange.hpp"
#include "portfolio.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include "helper.hpp"

using namespace teamA;

namespace teamA {
    TimeSeries FilterPrices(const TimeSeries data, const std::vector<std::string> &date_filter) {
        TimeSeries filtered_prices;
        // Iterate through the lookup vector
        for (auto lookup_it = date_filter.begin(); lookup_it != date_filter.end(); ++lookup_it) {
            // Iterate through the data vector
            for (auto data_it = data.begin(); data_it != data.end(); ++data_it) {
                // Compare the current lookup string with the first element of the pair
                if (*lookup_it == data_it->first) {
                    filtered_prices.push_back(*data_it); // Add the matching pair to the result
                }
            }
        }
        return filtered_prices;
    }

    TimeSeries LogReturns(const TimeSeries prices) {
        // Ensure there are at least two prices to compute log returns
        if (prices.size() < 2) {
            throw std::invalid_argument("Not enough data to compute log returns.");
        }
        TimeSeries logReturns;
        for (size_t i = 0; i < prices.size(); ++i) {
            double log_return = std::log(prices[i].second / prices[i - 1].second);
            logReturns.push_back({prices[i].first, log_return});
        }
        logReturns.erase(logReturns.begin());
        return logReturns;
    }
    std::ostream& operator<<(std::ostream& os, const TimeSeries& ts) {
        size_t count = 0;
        for (const auto& pair : ts) {
            os << std::left<< std::setw(11) << pair.first << ": " << std::left <<std::setw(8) << std::fixed << std::setprecision(4) << pair.second << "\t";
            count++;
        }
        os << std::endl;
        return os;
    }
    // Overload operator<< for std::vector<double>
    std::ostream& operator<<(std::ostream& os, const std::vector<double>& vec) {
        size_t count = 0;
        for (const auto& value : vec) {
            os << std::left<< std::setw(7) << std::fixed << std::setprecision(4) << value;
            os << "\t";
            count++;
        }
        os << std::endl;
        return os;
    }
}
