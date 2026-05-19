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
};

class LinearRegression {
public:
    static LinearRegressionResult fit(const std::vector<std::array<float, 2>> &points);
};

#endif /* LinearRegression_hpp */
