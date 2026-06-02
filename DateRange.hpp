#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>


// DateRange class to represent a date range
class DateRange {
protected:
    std::string start_date = "";
    std::string end_date = "";

private:
    static std::vector<std::string> trading_days;
    int N; //Trading days window 2N+1 set by user
public:
    DateRange(const std::string &path, const int N_): start_date(), end_date(), N(N_) {
        if (trading_days.empty()) {
            loadTradingDays(path);
        }
    };

    DateRange(std::string start, std::string end): start_date(start), end_date(end) {
    }

    static void loadTradingDays(const std::string &file_path);

    // std::string getRefDate() const { return ref_date; };
    std::string getStartDate() const { return start_date; };
    std::string getEndDate() const { return end_date; };

    void setStartDate(std::string ref_date);

    void setEndDate(std::string ref_date);

    std::vector<std::string> TradingRange(int L);
};
