#include "portfolio.hpp"
#include "helper.hpp"
#include "DateRange.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <tuple>


using namespace teamA;

Stock::Stock(std::string ticker_, std::string earningsDate_, std::string periodEnding_, double estimatedEPS_,
             double reportedEPS_,
             double surprise_, double surprisePercent_)
    : ticker(ticker_), earningsDate(earningsDate_), periodEnding(periodEnding_), estimatedEPS(estimatedEPS_),
      reportedEPS(reportedEPS_),
      surprise(surprise_), surprisePercent(surprisePercent_) {
}

std::string Stock::getTicker() {
    return ticker;
}

void Stock::setPrices(const std::map<std::string, TimeSeries> &data, const int N_) {
    std::map<std::string, TimeSeries>::const_iterator it = data.find(ticker);
    if (it != data.end()) {
        prices = it->second;
    } else {
        std::cerr << "Ticker " << ticker << " not found in the retrieved data." << std::endl;
    }
    // Filter the prices based on the earnings date range
    DateRange range(earningsDate, 0);
    range.setStartDate(earningsDate);
    range.setEndDate(earningsDate);
    std::vector<std::string> date_filter = range.TradingRange(N_);
    prices = FilterPrices(prices, date_filter);
}

void Stock::setLogReturns() {
    logReturns = LogReturns(prices);
    cumulativeReturns.clear();
    double cumulative_return = 0.0;
    for (size_t i = 0; i < logReturns.size(); ++i) {
        cumulative_return = cumulative_return + logReturns[i].second;
        cumulativeReturns.push_back({logReturns[i].first, cumulative_return});
    }
}

void Stock::setAbnormalReturns(const std::map<std::string, TimeSeries> &data, std::string ref_ticker, const int N_) {
    std::map<std::string, TimeSeries>::const_iterator it = data.find(ref_ticker);
    TimeSeries benchmarkPrices;
    if (it != data.end()) {
        benchmarkPrices = it->second;
    } else {
        std::cerr << "Benchmark Ticker " << ref_ticker << " not found in the retrieved data." << std::endl;
    }
    DateRange range(earningsDate, 0);
    range.setStartDate(earningsDate);
    range.setEndDate(earningsDate);
    std::vector<std::string> date_filter = range.TradingRange(N_);
    benchmarkPrices = FilterPrices(benchmarkPrices, date_filter);
    TimeSeries benchmarkReturns = LogReturns(benchmarkPrices);
    abnormalReturns.clear();
    // Calculate the abnormal returns
    for (size_t i = 0; i < logReturns.size(); ++i) {
        double abnormal_return = logReturns[i].second - benchmarkReturns[i].second;
        abnormalReturns.push_back(abnormal_return);
    }
}

std::pair<std::string, std::string> Portfolio::parseEarningsCSV(std::string filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return std::make_pair("", "");
    }

    std::string maxDate = "0000-00-00";
    std::string minDate = "9999-99-99";
    // std::string maxDate = "0000-01-01";

    // std::clock_t start = std::clock();

    try {
        std::string line;
        std::getline(file, line); //to skip the header
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string ticker, earningsDate, periodEnding, estimatedString, reportedString, surpriseString,
                    surprisePercentString;

            std::getline(ss, ticker, ',');
            std::getline(ss, earningsDate, ',');
            std::getline(ss, periodEnding, ',');
            std::getline(ss, estimatedString, ',');
            std::getline(ss, reportedString, ',');
            std::getline(ss, surpriseString, ',');
            std::getline(ss, surprisePercentString, ',');

            double estimatedEPS = std::stod(estimatedString);
            double reportedEPS = std::stod(reportedString);
            double surprise = std::stod(surpriseString);
            double surprisePercent = std::stod(surprisePercentString);

            // std::cout << ticker << " " << earningsDate << " " << periodEnding << " " << estimatedEPS << " " <<
            //         reportedEPS << " " << surprise
            //         << " " << surprisePercent <<
            // std::endl;

            // checkDate(minDate, maxDate, earningsDate);
            if (earningsDate < minDate) minDate = earningsDate;
            if (earningsDate > maxDate) maxDate = earningsDate;

            Stock *stock = new Stock(ticker,
                                     earningsDate,
                                     periodEnding,
                                     estimatedEPS,
                                     reportedEPS,
                                     surprise,
                                     surprisePercent);
            if (stock != nullptr) {
                stocks.push_back(stock);
            } else {
                std::cerr << "Error creating stock object for ticker: " << ticker << std::endl;
                continue;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
    }

    file.close();

    // std::clock_t end = std::clock();

    // std::cout << double(end - start) / CLOCKS_PER_SEC << std::endl;

    return std::make_pair(minDate, maxDate);
}

void Portfolio::setAAR() {
    if (stocks.empty()) return;
    std::vector<double> sum;
    if (stocks.front()->getAbnormalReturns().size() == expectedSize) {
        sum = stocks.front()->getAbnormalReturns();
    }
    for (size_t i = 1; i < stocks.size(); ++i) {
        if (stocks[i]->getAbnormalReturns().size() != expectedSize) {
            continue;
        }
        if (sum.empty()) {
            sum = stocks[i]->getAbnormalReturns(); // Initialize sum if it was empty
        } else {
            sum = sum + stocks[i]->getAbnormalReturns();
        }
    }
    AAR = sum / static_cast<double>(stocks.size());
}

void Portfolio::setCAAR() {
    if (AAR.empty()) {
        setAAR();
    }
    CAAR = AAR;
    for (size_t i = 1; i < AAR.size(); ++i) {
        CAAR[i] += CAAR[i - 1];
    }
}

void Portfolio::addStock(Stock *stock) {
    stocks.push_back(stock);
}

std::vector<std::string> Portfolio::getTickerList() {
    std::vector<std::string> tickers;
    for (Stock *stock: stocks) {
        tickers.push_back(stock->getTicker());
    }
    return tickers;
}

void Portfolio::setStockData(const std::map<std::string, TimeSeries> &data, const int N_,
                             const std::string &ref_ticker) {
    for (Stock *stock: stocks) {
        stock->setPrices(data, N_);
        stock->setLogReturns();
        stock->setAbnormalReturns(data, ref_ticker, N_);
    }
}

void Portfolio::removeStocks(std::unordered_set<std::string> tickers) {
    auto it = std::remove_if(stocks.begin(), stocks.end(),
                             [&](Stock *stock) {
                                 if (tickers.count(stock->getTicker())) {
                                     delete stock; // Free memory
                                     return true; // Remove this stock
                                 }
                                 return false;
                             });
    stocks.erase(it, stocks.end());
}



