#include "PortfolioClassifier.hpp"
#include "portfolio.hpp"
#include <algorithm>

using namespace teamA;

PortfolioClassifier::PortfolioClassifier(const Portfolio &portfolio)
    : originalPortfolio(portfolio) {
}

void PortfolioClassifier::classify(const std::string& ref_ticker) {
    std::vector<Stock *> allStocks = originalPortfolio.getStocks();

    // Remove null pointers
    allStocks.erase(std::remove(allStocks.begin(), allStocks.end(), nullptr), allStocks.end());
    // Remove the Stock* with the ticker matching ref_ticker
    allStocks.erase(std::remove_if(allStocks.begin(), allStocks.end(),
    [&ref_ticker](Stock* stock) {
        return stock->getTicker() == ref_ticker;
    }),
    allStocks.end());

    // Sort stocks by surprisePercent in ascending order
    std::sort(allStocks.begin(), allStocks.end(), [](Stock *a, Stock *b) {
        return a->getSurprisePercent() < b->getSurprisePercent();
    });

    size_t total = allStocks.size();
    size_t third = total / 3;
    missPortfolio.setStocks(std::vector<Stock *>(allStocks.begin(), allStocks.begin() + third));
    meetPortfolio.setStocks(std::vector<Stock *>(allStocks.begin() + third, allStocks.begin() + 2 * third));
    beatPortfolio.setStocks(std::vector<Stock *>(allStocks.begin() + 2 * third, allStocks.end()));
}

Portfolio PortfolioClassifier::getMissPortfolio() const {
    return missPortfolio;
}

Portfolio PortfolioClassifier::getMeetPortfolio() const {
    return meetPortfolio;
}

Portfolio PortfolioClassifier::getBeatPortfolio() const {
    return beatPortfolio;
}

void PortfolioClassifier::setGroupForPortfolios() {
    std::vector<Stock *> missStocks = missPortfolio.getStocks();
    missStocks.erase(std::remove(missStocks.begin(), missStocks.end(), nullptr), missStocks.end());
    std::vector<Stock *>::iterator it = missStocks.begin();
    for (; it != missStocks.end(); ++it) {
        (*it)->setGroup("Miss Earnings");
    }
    std::vector<Stock *> meetStocks = meetPortfolio.getStocks();
    meetStocks.erase(std::remove(meetStocks.begin(), meetStocks.end(), nullptr), meetStocks.end());
    it = meetStocks.begin();
    for (; it != meetStocks.end(); ++it) {
        (*it)->setGroup("Meet Earnings");
    }
    std::vector<Stock *> beatStocks = beatPortfolio.getStocks();
    beatStocks.erase(std::remove(beatStocks.begin(), beatStocks.end(), nullptr), beatStocks.end());
    it = beatStocks.begin();
    for (; it != beatStocks.end(); ++it) {
        (*it)->setGroup("Beat Earnings");
    }
}

PortfolioClassifier::~PortfolioClassifier() {
    for (Stock *stock: beatPortfolio.getStocks()) {
        delete stock;
    }
    for (Stock *stock: meetPortfolio.getStocks()) {
        delete stock;
    }
    for (Stock *stock: missPortfolio.getStocks()) {
        delete stock;
    }
    beatPortfolio.setStocks(std::vector<Stock *>());
    meetPortfolio.setStocks(std::vector<Stock *>());
    missPortfolio.setStocks(std::vector<Stock *>());
}

