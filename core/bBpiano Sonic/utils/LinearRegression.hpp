//
//  LinearRegression.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#ifndef LinearRegression_hpp
#define LinearRegression_hpp

#include <iostream>

struct LinearRegressionResult {
    double k = 0.0;
    double b = 0.0;
    double r2 = 0.0;
    std::size_t n = 0;
    
    LinearRegressionResult(double _k, double _b, double _r2, std::size_t _n) : k(_k), b(_b), r2(_r2), n(_n) {};
    LinearRegressionResult(){};
};

struct QuadraticRegressionResult {
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double r2 = 0.0;
    std::size_t n = 0;
    
    QuadraticRegressionResult(double _a, double _b, double _c, double _r2, std::size_t _n)
    : a(_a), b(_b), c(_c), r2(_r2), n(_n) {};
    QuadraticRegressionResult(){};
};

class LinearRegression {
public:
    static LinearRegressionResult fit(const std::vector<std::array<float, 2>> &points);
    static LinearRegressionResult fit(const std::vector<std::array<double, 2>> &points);
    static QuadraticRegressionResult fit2(const std::vector<std::array<float, 2>> &points);
    static QuadraticRegressionResult fit2(const std::vector<std::array<double, 2>> &points);
};

#endif /* LinearRegression_hpp */
