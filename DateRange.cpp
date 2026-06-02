#include "DateRange.hpp"
#include <iostream>

std::vector<std::string> DateRange::trading_days;

std::string cleanLine(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n,"); // Also remove trailing commas
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// Static method to load trading days from a CSV file
void DateRange::loadTradingDays(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open trading days file: " + file_path);
    }
    std::string line;
    while (getline(file, line)) {
        line = cleanLine(line);
        trading_days.push_back(line);
    }
    file.close();
};

void DateRange::setStartDate(std::string ref_date) {
    if (ref_date.empty()) {
        throw std::invalid_argument("Reference date cannot be an empty string.");
    }

    std::vector<std::string>::iterator it = find(trading_days.begin(), trading_days.end(), ref_date);

    if (it == trading_days.end()) {
        // If ref_date is not found, find the closest date
        it = std::lower_bound(trading_days.begin(), trading_days.end(), ref_date);
        if (it == trading_days.end()) {
            --it; // If ref_date is greater than all dates, use the last date
        }
    }
    // Set start_date and end_date based on the reference date
    if (it != trading_days.begin()) {
        start_date = *(it - (N + 1)); // Previous trading day
    } else {
        start_date = *it; // First trading day
    }
};

void DateRange::setEndDate(std::string ref_date) {
    if (ref_date.empty()) {
        throw std::invalid_argument("Reference date cannot be an empty string.");
    }

    std::vector<std::string>::iterator it = find(trading_days.begin(), trading_days.end(), ref_date);

    if (it == trading_days.end()) {
        // If ref_date is not found, find the closest date
        it = std::lower_bound(trading_days.begin(), trading_days.end(), ref_date);
        if (it == trading_days.end()) {
            --it; // If ref_date is greater than all dates, use the last date
        }
    }
    // Set start_date and end_date based on the reference date
    if (it != trading_days.end()) {
        end_date = *(it + (N+1)); // Previous trading day
    } else {
        end_date = *it; // Last trading day
    }
};

std::vector<std::string> DateRange::TradingRange(int L) {
    std::vector<std::string> range;
    std::vector<std::string>::iterator it_start = find(trading_days.begin(), trading_days.end(), start_date);
    std::vector<std::string>::iterator it_end = find(trading_days.begin(), trading_days.end(), end_date);
    range.assign(it_start - L, it_end + L);
    // doesnt check if it_start-L and it_end+L+1 is out of bounds
    return range;
};
