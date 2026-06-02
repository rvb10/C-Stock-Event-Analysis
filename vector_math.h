#pragma once
#include <vector>

/*----------- element‑wise binary operators (vector–vector) ----------*/
std::vector<double>& operator+=(std::vector<double>& a,
                                const std::vector<double>& b);
std::vector<double>& operator-=(std::vector<double>& a,
                                const std::vector<double>& b);

std::vector<double>  operator+(const std::vector<double>& a,
                               const std::vector<double>& b);
std::vector<double>  operator-(const std::vector<double>& a,
                               const std::vector<double>& b);

/*----------- vector–scalar products --------------------------------*/
std::vector<double>& operator*=(std::vector<double>& v, double s);
std::vector<double>& operator/=(std::vector<double>& v, double s);

std::vector<double>  operator*(const std::vector<double>& v, double s);
std::vector<double>  operator*(double s, const std::vector<double>& v);
std::vector<double>  operator/(const std::vector<double>& v, double s);

/*----------- Element‑wise product ---------------------------*/
std::vector<double>& operator*(std::vector<double>& a, const std::vector<double>& b);
std::vector<double> operator*(const std::vector<double>& a, const std::vector<double>& b);
std::vector<double>& operator/(std::vector<double>& a, const std::vector<double>& b);
std::vector<double> operator/(const std::vector<double>& a, const std::vector<double>& b);



