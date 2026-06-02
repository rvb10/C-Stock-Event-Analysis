#pragma once
#include <string>
#include <map>
#include <memory>
#include <unordered_set>

#include "helper.hpp"
#include "DateRange.hpp"
#include "vector_math.h"

namespace teamA {
    class Stock {
        std::string ticker;
        std::string earningsDate;
        std::string periodEnding;
        std::pair<std::string, std::string> earningsRange;
        double estimatedEPS = 0.0;
        double reportedEPS = 0.0;
        double surprise = 0.0;
        double surprisePercent = 0.0;
        std::map<std::string, double> priceHistory = {};

    protected:
        std::string group;
        TimeSeries prices;
        TimeSeries logReturns;
        TimeSeries cumulativeReturns;
        std::vector<double> abnormalReturns;

    public:
        Stock(std::string ticker_, std::string earningsDate_, std::string periodEnding_, double estimatedEPS_,
              double reportedEPS_, double surprise_,
              double surprisePercent_);

        void setPrices(const std::map<std::string, TimeSeries> &data, const int N_);

        void setLogReturns();

        void setAbnormalReturns(const std::map<std::string, TimeSeries> &data, std::string ref_ticker, const int N_);

        std::string getTicker();

        TimeSeries getPrices() { return prices; };
        void setGroup(std::string group_) { group = group_; }
        std::string getGroup() { return group; }
        double getEstimatedEPS() { return estimatedEPS; }
        double getReportedEPS() { return reportedEPS; }
        double getSurprisePercent() { return surprisePercent; }
        double getSurprise() { return surprise; }
        std::string getEarningsDate() { return earningsDate; }
        std::string getPeriodEnding() { return periodEnding; }
        TimeSeries getLogReturns() { return logReturns; };
        TimeSeries getCumulativeReturns() { return cumulativeReturns; };
        std::vector<double> getAbnormalReturns() { return abnormalReturns; };
    };

    class Portfolio {
        std::vector<Stock *> stocks;
        std::vector<double> AAR;
        std::vector<double> CAAR;
        int expectedSize;

    public:
        void setAAR();

        void setCAAR();

        void setExpectedSize(int size) { expectedSize = size; }

        Portfolio() : expectedSize(0) {
        }

        Portfolio(int expectedSize_) : expectedSize(expectedSize_) {
        }

        std::vector<double> getARR() const { return AAR; }
        std::vector<double> getCAAR() const { return CAAR; }
        std::vector<Stock *> getStocks() const { return stocks; }
        void setStocks(const std::vector<Stock *> &s) { stocks = s; }

        void setStockData(const std::map<std::string, TimeSeries> &data, const int N_, const std::string &ref_ticker);

        std::pair<std::string, std::string> parseEarningsCSV(std::string filePath);

        void removeStocks(std::unordered_set<std::string> tickers);

        std::vector<std::string> getTickerList();

        void addStock(Stock *stock);
    };
};


