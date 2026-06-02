#include "Retrieve.hpp"
#include "portfolio.hpp"
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>
#include <mutex>
#include <unordered_set>

using namespace teamA;

// Initialize libcurl global environment
// void StockDataFetcher::init() {
//
// }

const std::string StockDataFetcher::api_token =
        "68101dc7b688a8.52043030";

StockDataFetcher::StockDataFetcher() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

StockDataFetcher::~StockDataFetcher() {
    curl_global_cleanup();
}


// Clean up resources used by libcurl
// void StockDataFetcher::cleanup() {
//     curl_global_cleanup();
// }

// Builds the URL required for the EOD Historical Data API request
string StockDataFetcher::makeURL(const string &ticker, const string &start_date, const string &end_date) {
    ostringstream oss;

    oss << "https://eodhistoricaldata.com/api/eod/" << ticker
            << "?api_token=" << api_token
            << "&from=" << start_date
            << "&to=" << end_date
            << "&fmt=csv";
    return oss.str();
}

// This function is used with libcurl to handle data received during HTTPS request
size_t StockDataFetcher::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string *) userp)->append((char *) contents, size * nmemb); // Says how to append the data as it arrives in chunks
    return size * nmemb;
}

// Makes an HTTP GET request to fetch the stock data as CSV
string StockDataFetcher::httpGET(const string &url) {
    CURL *curl = curl_easy_init();
    string readBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Some curl options (URL)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // Destination
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Curl error: " << curl_easy_strerror(res) << endl;
            readBuffer.clear();
        }
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

// Fetches stock data for a list of tickers within a date range
map<string, TimeSeries> StockDataFetcher::fetch(
    Portfolio *portfolio,
    const string &start_date,
    const string &end_date,
    int N,
    std::function<void(float)> reportProgress) {
    auto start_time = std::chrono::high_resolution_clock::now();
    vector<string> tickers = portfolio->getTickerList();
    // Global token‑bucket rate limiter: one request every 60ms
    std::map<std::string, TimeSeries> data;

    size_t total = tickers.size();
    std::atomic<size_t> completed{0};
    std::mutex progress_mutex;
    std::mutex fail_mutex;
    std::mutex success_mutex;
    std::unordered_set<std::string> failedTickers;
    static std::mutex rate_mtx;
    static auto nextAllowed = std::chrono::steady_clock::now();

    // Determine number of threads to use
    unsigned int thread_num = std::thread::hardware_concurrency();
    if (thread_num == 0) thread_num = 1;
    thread_num = std::min(thread_num, static_cast<unsigned int>(total));

    // Compute chunk size
    size_t chunk = (total + thread_num - 1) / thread_num;

    // Launch worker threads
    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < thread_num; ++i) {
        size_t start_idx = i * chunk;
        size_t end_idx = std::min(start_idx + chunk, total);

        //Populate worker threads vector via threads with anonymous functions
        workers.emplace_back(
            [ &tickers, start_date, end_date, &data, &reportProgress, &completed, total, start_idx, end_idx,
                &progress_mutex, &fail_mutex, &failedTickers, N, &success_mutex ]() {
                for (size_t j = start_idx; j < end_idx; ++j) {
                    const auto &ticker = tickers[j];
                    std::string url = makeURL(ticker, start_date, end_date);
                    //Rate limit via global token bucket
                    {
                        std::lock_guard<std::mutex> lock(rate_mtx);
                        auto now = std::chrono::steady_clock::now();
                        if (now < nextAllowed) {
                            std::this_thread::sleep_until(nextAllowed);
                        }
                        nextAllowed += std::chrono::milliseconds(60);
                    }

                    std::string response = httpGET(url);
                    if (!response.empty()) {
                        auto ts = parseCSV(response);

                        if (ts.size() >= (2 * N) + 1) {
                            std::lock_guard<std::mutex> success_lock(success_mutex);
                            data[ticker] = std::move(ts);
                        } else {
                            std::lock_guard<std::mutex> fail_lock(fail_mutex);
                            failedTickers.insert(ticker);
                        }
                    } {
                        std::lock_guard<std::mutex> plock(progress_mutex);
                        size_t done = ++completed;
                        reportProgress(static_cast<float>(done) / total);
                    }
                }
            });
    }

    // Wait for all threads to finish
    for (auto &th: workers) {
        th.join();
    }

    //Removing those stocks from portfolio that don't have sufficient price data
    portfolio->removeStocks(failedTickers);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << std::endl << "Fetch completed in " << elapsed.count() << " seconds." << std::endl;

    std::cout << endl;
    for (auto fail: failedTickers) {
        std::cout << "~ " << fail << " did not have sufficient data." << std::endl;
    }
    std::cout << std::endl;
    return data;
}

// Parses the CSV string containing stock data from EOD and converts it into a price history
TimeSeries StockDataFetcher::parseCSV(const string &csv) {
    istringstream ss(csv);
    string line;
    TimeSeries history;

    getline(ss, line); // Skip header

    while (getline(ss, line)) {
        istringstream line_ss(line);
        string date, tmp;
        double adj_close = 0.0;
        int col = 0;

        while (getline(line_ss, tmp, ',')) {
            if (col == 0) date = tmp;
            if (col == 5) adj_close = stod(tmp); // The sixth column is the adjusted close price
            ++col;
        }

        if (!date.empty()) {
            history.emplace_back(date, adj_close);
        }
    }

    return history;
}
