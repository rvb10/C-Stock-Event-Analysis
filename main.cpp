#include <iostream>
#include <string>

#include "DateRange.hpp"
#include "portfolio.hpp"
#include "Retrieve.hpp"
#include "helper.hpp"
#include "PortfolioClassifier.hpp"
#include "Bootstrapping.hpp"
#include "plot_helper.h"


using namespace teamA;

struct Session {
    int N = 0;
    int sim_ = 40;
    int sample_ = 30;
    bool isDataRetrieved = false;
    Portfolio *portfolio = nullptr;
    DateRange *dateRange = nullptr;
    StockDataFetcher fetcher;
    PortfolioClassifier *classifier = nullptr;
    Portfolio miss, meet, beat;
    Bootstrap miss_sim, meet_sim, beat_sim;
    std::vector<double> miss_cavg, meet_cavg, beat_cavg;
    std::vector<double> miss_avg, meet_avg, beat_avg;
    std::vector<std::vector<double> > group_matrix;
};

void showMenu();

bool retrieveData(Session &s);

void showSingleStock(Session &s);

void showSingleGroup(Session &s);

void plotCAAR(Session &s);

void noData();

void cleanup(Session &s) {
    delete s.portfolio;
    delete s.dateRange;
    delete s.classifier;
}


int main() {
    int option = 0;
    bool running = true;
    bool isDataRetrieved = false;
    Session s;

    while (running) {
        showMenu();
        std::cin >> option;
        std::cout << "\n";

        // Check for input failure
        if (std::cin.fail()) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(10000, '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(10000, '\n');
        //flush after reading option, for float inputs like "1.7" for example will read "1".
        switch (option) {
            case 1:
                isDataRetrieved = retrieveData(s);
                break;
            case 2:
                if (!isDataRetrieved) {
                    noData();
                    break;
                }
                showSingleStock(s);
                break;
            case 3:
                if (!isDataRetrieved) {
                    noData();
                    break;
                }
                showSingleGroup(s);
                break;
            case 4:
                if (!isDataRetrieved) {
                    noData();
                    break;
                }
                plotCAAR(s);
                break;
            case 5:
                std::cout << "Exiting...\n";
                running = false;
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    }
    cleanup(s);
    return 0;
}


void showMenu() {
    std::cout << "\n===== MAIN MENU =====\n";
    std::cout << "Press 1: Retrieve historical data.\n";
    std::cout << "Press 2: Show single stock summary.\n";
    std::cout << "Press 3: Show single group metrics.\n";
    std::cout << "Press 4: Plot Cumulative Average Abnormal Returns (CAAR).\n";
    std::cout << "5: Exit\n";
    std::cout << "Select an option: ";
}

bool retrieveData(Session &s) {
    std::cout <<
            "Enter a positive interger between 30 and 60 to retrieve 2N+1 days of historical price data of Russell 3000 stocks.\n"
            "To go back press 0.\n";
    int N_input = 0;
    while (true) {
        std::cin >> N_input;
        // Check if input failed (e.g., user typed letters instead of an integer)
        if (std::cin.fail()) {
            std::cin.clear(); // Clear error state
            std::cin.ignore(10000, '\n'); // Discard invalid input
            std::cout << "That's not a valid number. Try again.\n";
            continue;
        }
        std::cin.ignore(10000, '\n'); // flush after reading option

        // Go back to menu
        if (N_input == 0) {
            return false;
        }

        // Check if N_input is within the required range
        if (N_input < 30 || N_input > 60) {
            std::cout << "Number out of range. Try again.\n";
            continue;
        }
        // Retrieving data and initializing objects
        s.N = N_input;
        std::cout << "Retrieving data..." << std::endl;
        s.portfolio = new Portfolio(2 * s.N + 1);
        std::pair<std::string, std::string> dates_pair = s.portfolio->parseEarningsCSV(
            "./data/Russell3000EarningsAnnouncements.csv");


        s.dateRange = new DateRange("./data/TradingDaysCalendar.csv", s.N + 1);
        s.dateRange->setStartDate(dates_pair.first);
        s.dateRange->setEndDate(dates_pair.second);
        std::cout << s.dateRange->TradingRange(0).size();

        StockDataFetcher fetcher;
        std::map<string, TimeSeries> stockPrices;


        try {
            runWithProgressBar([&](std::function<void(float)> reportProgress) {
                stockPrices = fetcher.fetch(s.portfolio, s.dateRange->getStartDate(),
                                            s.dateRange->getEndDate(), s.N, reportProgress);
            });
        } catch (std::invalid_argument &e) {
            std::cerr << "Request failed: HTTP 429 Too Many Requests – API rate limit exceeded." << std::endl;
        }
        s.portfolio->setStockData(stockPrices, s.N, "IWV");
        s.classifier = new PortfolioClassifier(*s.portfolio);
        s.classifier->classify("IWV");
        s.classifier->setGroupForPortfolios();
        s.miss = s.classifier->getMissPortfolio();
        s.meet = s.classifier->getMeetPortfolio();
        s.beat = s.classifier->getBeatPortfolio();

        s.miss_sim.BootstrapSim(s.miss, s.sample_, s.sim_, s.N);
        s.meet_sim.BootstrapSim(s.meet, s.sample_, s.sim_, s.N);
        s.beat_sim.BootstrapSim(s.beat, s.sample_, s.sim_, s.N);

        s.miss_sim.setAvgs();
        s.meet_sim.setAvgs();
        s.beat_sim.setAvgs();
        s.miss_sim.setSTD();
        s.meet_sim.setSTD();
        s.beat_sim.setSTD();

        s.miss_avg = s.miss_sim.getAvgARR();
        s.meet_avg = s.meet_sim.getAvgARR();
        s.beat_avg = s.beat_sim.getAvgARR();
        s.miss_cavg = s.miss_sim.getAvgCAAR();
        s.meet_cavg = s.meet_sim.getAvgCAAR();
        s.beat_cavg = s.beat_sim.getAvgCAAR();

        s.group_matrix = std::vector<std::vector<double> >(3, std::vector<double>(4));
        s.group_matrix[0][0] = s.miss_sim.getFinalAvgARR();
        s.group_matrix[0][1] = s.miss_sim.getFinalStdARR();
        s.group_matrix[0][2] = s.miss_sim.getFinalAvgCAAR();
        s.group_matrix[0][3] = s.miss_sim.getFinalStdCAAR();
        s.group_matrix[1][0] = s.meet_sim.getFinalAvgARR();
        s.group_matrix[1][1] = s.meet_sim.getFinalStdARR();
        s.group_matrix[1][2] = s.meet_sim.getFinalAvgCAAR();
        s.group_matrix[1][3] = s.meet_sim.getFinalStdCAAR();
        s.group_matrix[2][0] = s.beat_sim.getFinalAvgARR();
        s.group_matrix[2][1] = s.beat_sim.getFinalStdARR();
        s.group_matrix[2][2] = s.beat_sim.getFinalAvgCAAR();
        s.group_matrix[2][3] = s.beat_sim.getFinalStdCAAR();

        break;
    }
    return true;
}

void showSingleStock(Session &s) {
    std::cout << "Enter a valid ticker. To go back press 0.\n";
    std::string ticker_input;

    while (true) {
        std::cin >> ticker_input;

        // Go back to menu
        if (ticker_input == "0") {
            return;
        }
        bool found = false;
        for (Stock *stock: s.portfolio->getStocks()) {
            if (stock != nullptr && stock->getTicker() == ticker_input) {
                // Print the data members of the matching Stock*
                std::cout << "-------------------------\n";
                std::cout << "Stock Information:\n";
                std::cout << "-------------------------\n";
                std::cout << "Ticker: " << stock->getTicker() << std::endl;
                std::cout << "Group: " << stock->getGroup() << std::endl;
                std::cout << std::endl;
                std::cout << "Earning for Period Ending: " << stock->getPeriodEnding() << std::endl;
                std::cout << "Earnings Date Announcement: " << stock->getEarningsDate() << std::endl;
                std::cout << "Estimated EPS: " << stock->getEstimatedEPS() << std::endl;
                std::cout << "Reported EPS: " << stock->getReportedEPS() << std::endl;
                std::cout << "Surprise: " << stock->getSurprise() << std::endl;
                std::cout << "Surprise Percent: " << stock->getSurprisePercent() << std::endl;
                std::cout << std::endl;
                std::cout << "-------------------------\n";
                std::cout << "Daily Prices: (" << stock->getPrices().size() << " obs.)" << std::endl;
                std::cout << "-------------------------\n";
                std::cout << std::endl;
                std::cout << stock->getPrices() << std::endl;
                std::cout << std::endl;
                std::cout << "-------------------------\n";
                std::cout << "Log Returns: (" << stock->getLogReturns().size() << " obs.)" << std::endl;
                std::cout << "-------------------------\n";
                std::cout << std::endl;
                std::cout << stock->getLogReturns() << std::endl;
                std::cout << std::endl;
                std::cout << "-------------------------\n";
                std::cout << "Cumulative Returns: (" << stock->getCumulativeReturns().size() << " obs.)" << std::endl;
                std::cout << "-------------------------\n";
                std::cout << std::endl;
                std::cout << stock->getCumulativeReturns() << std::endl;
                std::cout << "-------------------------\n";
                std::cout << "Abnormal Returns: (" << stock->getAbnormalReturns().size() << " obs.)" << std::endl;
                std::cout << "-------------------------\n";
                std::cout << std::endl;
                std::cout << stock->getAbnormalReturns() << std::endl;
                found = true;
                // Valid input
                break;
            }
        }
        if (!found) {
            std::cout << "Ticker not found in the portfolio. Try again or press 0 to go back.\n";
        } else {
            break; // Exit the loop after printing the stock data
        }
    }
}

void showSingleGroup(Session &s) {
    std::cout <<
            "Enter a valid option:\n Enter 1 for Beat group.\n Enter 2 for Meet group.\n Enter 3 for Miss group.\n Enter 4 for Summary Matrix.\n";
    std::cout << "To go back press 0.\n";
    int group_input;

    while (true) {
        std::cin >> group_input;

        // Check if input failed (e.g., user typed letters instead of an integer)
        if (std::cin.fail()) {
            std::cin.clear(); // Clear error state
            std::cin.ignore(10000, '\n'); // Discard invalid input
            std::cout << "That's not a valid number. Try again.\n";
            continue;
        }
        std::cin.ignore(10000, '\n'); // flush after reading option

        // Go back to menu
        if (group_input == 0) {
            return;
        }

        // Check if N_input is within the required range
        if (group_input < 0 || group_input > 4) {
            std::cout << "Number out of range. Try again.\n";
            continue;
        }
        // Valid input
        break;
    }
    if (group_input == 1) {
        std::cout << "Beat group Information: (" << s.beat.getStocks().size() << " Stocks)" << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "Beat: Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.beat_sim.getAvgARR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Average Abnormal Returns Standard Deviation: " << s.beat_sim.getCAARstd().size() << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.beat_sim.getARRstd() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Beat: Cumulative Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.beat_sim.getAvgCAAR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Beat: Cumulative Average Abnormal Returns Standard Deviation: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.beat_sim.getCAARstd() << std::endl;
        std::cout << std::endl;
    } else if (group_input == 2) {
        std::cout << "Meet group Information: (" << s.meet.getStocks().size() << " Stocks)" << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "Meet: Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.meet_sim.getAvgARR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Meet: Average Abnormal Returns Standard Deviation: " << s.meet_sim.getCAARstd().size() <<
                std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.meet_sim.getARRstd() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Meet: Cumulative Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.meet_sim.getAvgCAAR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Meet: Cumulative Average Abnormal Returns Standard Deviation: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.meet_sim.getCAARstd() << std::endl;
        std::cout << std::endl;
    } else if (group_input == 3) {
        std::cout << "Miss group Information: (" << s.miss.getStocks().size() << " Stocks)" << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "Miss: Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.miss_sim.getAvgARR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Miss: Average Abnormal Returns Standard Deviation: " << s.miss_sim.getCAARstd().size() <<
                std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.miss_sim.getARRstd() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Miss: Cumulative Average Abnormal Returns: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.miss_sim.getAvgCAAR() << std::endl;
        std::cout << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "Miss: Cumulative Average Abnormal Returns Standard Deviation: " << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << s.miss_sim.getCAARstd() << std::endl;
        std::cout << std::endl;
    }
    if (group_input == 4) {
        std::cout << "Group Metrics:\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << std::left << std::setw(15) << "Group"
                << std::setw(8) << "AvgARR"
                << std::setw(8) << "StdARR"
                << std::setw(8) << "AvgCAAR"
                << std::setw(8) << "StdCAAR" << "\n";

        std::cout << "-------------------------------------------------------\n";
        std::vector<std::string> groupNames = {"Miss group", "Meet group", "Beat group"};
        for (size_t i = 0; i < s.group_matrix.size(); ++i) {
            std::cout << std::left << std::setw(15) << groupNames[i]
                    << std::setw(8) << std::fixed << std::setprecision(4) << s.group_matrix[i][0]
                    << std::setw(8) << std::fixed << std::setprecision(4) << s.group_matrix[i][1]
                    << std::setw(8) << std::fixed << std::setprecision(4) << s.group_matrix[i][2]
                    << std::setw(8) << std::fixed << std::setprecision(4) << s.group_matrix[i][3] << "\n";
        }
        std::cout << "-------------------------------------------------------\n";
    }
}

void plotCAAR(Session &s) {
    //Call for plot function.
    std::cout << "Showing plot..\n";
    PlotThreeVectors(s.beat_cavg, s.meet_cavg, s.miss_cavg, s.N);
}

void noData() {
    std::cout << "Data has not been retrieved. Go to option 1 first. \n";
}
