#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "portfolio.hpp"
#include <sstream>
#include <curl/curl.h>

using namespace std;
using namespace teamA;

class StockDataFetcher {
    const static string api_token; // Free account gives 20 calls a day with 500 as welcome bonus
public:
    // Initialize libcurl global environment
    static void init();

    // Clean up resources used by libcurl
    static void cleanup();

    // Builds the URL required for the EOD Historical Data API request
    static string makeURL(const string &ticker, const string &start_date, const string &end_date);

    // This function is used with libcurl to handle data received during HTTPS request
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    // Makes an HTTP GET request to fetch the stock data as CSV
    static string httpGET(const string &url);

    // Fetches stock data for a list of tickers within a date range
    static map<string, TimeSeries> fetch(
        Portfolio *portfolio,
        const string &start_date,
        const string &end_date,
        int N,
        std::function<void(float)> reportProgress);

    // Parses the CSV string containing stock data from EOD and converts it into a price history
    static TimeSeries parseCSV(const string &csv);

    StockDataFetcher();

    ~StockDataFetcher();
};


