#pragma once
#include "portfolio.hpp"

class PortfolioClassifier {
private:
    teamA::Portfolio originalPortfolio;
    teamA::Portfolio missPortfolio;
    teamA::Portfolio meetPortfolio;
    teamA::Portfolio beatPortfolio;

public:
    PortfolioClassifier(const teamA::Portfolio &portfolio);

    void classify(const std::string& ref_ticker);
    void setGroupForPortfolios();
    teamA::Portfolio getMissPortfolio() const;
    teamA::Portfolio getMeetPortfolio() const;
    teamA::Portfolio getBeatPortfolio() const;
    ~PortfolioClassifier();
};