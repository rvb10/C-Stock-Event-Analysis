#include "vector_math.h"

/*----------- element‑wise addition / subtraction -----------------------*/
std::vector<double>& operator+=(std::vector<double>& a,
                                const std::vector<double>& b)
{
    for (std::size_t i = 0; i < a.size(); ++i) a[i] += b[i];
    return a;
}
std::vector<double>& operator-=(std::vector<double>& a,
                                const std::vector<double>& b)
{
    for (std::size_t i = 0; i < a.size(); ++i) a[i] -= b[i];
    return a;
}
std::vector<double> operator+(const std::vector<double>& a,
                              const std::vector<double>& b)
{   std::vector<double> res = a; return res += b; }

std::vector<double> operator-(const std::vector<double>& a,
                              const std::vector<double>& b)
{   std::vector<double> res = a; return res -= b; }

/*----------- vector–scalar products -----------------------------------*/
std::vector<double>& operator*=(std::vector<double>& v, double s)
{
    for (double& x : v) x *= s;
    return v;
}
std::vector<double>& operator/=(std::vector<double>& v, double s)
{
    if (s == 0.0) throw std::invalid_argument("Division by zero.");
    for (double& x : v) x /= s;
    return v;
}
std::vector<double> operator*(const std::vector<double>& v, double s)
{   std::vector<double> res = v; return res *= s; }

std::vector<double> operator*(double s, const std::vector<double>& v)
{   return v * s; }

std::vector<double> operator/(const std::vector<double>& v, double s)
{   std::vector<double> res = v; return res /= s; }

/*----------- Element‑wise product ---------------------------*/
std::vector<double>& operator*(std::vector<double>& a,
                               const std::vector<double>& b)
{
    for (std::size_t i = 0; i < a.size(); ++i) a[i] *= b[i];
    return a;
}
std::vector<double> operator*(const std::vector<double>& a,
                              const std::vector<double>& b)
{
    std::vector<double> res = a;
    for (std::size_t i = 0; i < a.size(); ++i) res[i] *= b[i];
    return res;
}
std::vector<double>& operator/(std::vector<double>& a,
                               const std::vector<double>& b)
{
    for (std::size_t i = 0; i < a.size(); ++i) a[i] /= b[i];
    return a;
}
std::vector<double> operator/(const std::vector<double>& a,
                              const std::vector<double>& b)
{
    std::vector<double> res = a;
    for (std::size_t i = 0; i < a.size(); ++i) res[i] /= b[i];
    return res;
}

