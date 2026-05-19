//
//  LinearRegression.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#include <array>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "LinearRegression.hpp"

LinearRegressionResult LinearRegression::fit(const std::vector<std::array<float, 2>> &points) {
    
    double k = 0.0;
    double b = 0.0;
    double r2 = 0.0;
    const std::size_t n = points.size();
    
    if (n < 2)
        throw std::invalid_argument("线形回归至少需要两个点吧");

    double sum_x = 0.0;
    double sum_y = 0.0;
    
    double sum_xy = 0.0;
    
    double sum_x2 = 0.0;
    double sum_y2 = 0.0;
    
    // constants
    for(const auto& p : points) {
        // 使用 at 不越界
        const double x = static_cast<double>(p[0]);
        const double y = static_cast<double>(p[1]);

        sum_x += x;
        sum_y += y;
        
        sum_xy += x * y;
        
        sum_x2 += x * x;
        sum_y2 += y * y;
    }
    
    double denom_x = static_cast<double>(n) * sum_x2 - sum_x * sum_x;
    if (std::abs(denom_x) < 1e-12) {
        throw std::runtime_error("不能回归出结果，因为 x 都太相似了，这会导致分母太接近 0");
    }
    const double numerator = static_cast<double>(n) * sum_xy - sum_x * sum_y;
    
    // k
    // k=\frac{n\sum x_iy_i-\sum x_i\sum y_i}{n\sum x_i^2-(\sum x_i)^2}
    k = numerator / denom_x;
    
    // b
    // b=\frac{\sum y_i-k\sum x_i}{n}
    b = (sum_y - k * sum_x) / static_cast<double>(n);
    
    // r2
    // r^2=\frac{(n\sum xy-\sum x\sum y)^2}{(n\sum x^2-(\sum x)^2)(n\sum y^2-(\sum y)^2)}
    const double denom_y = static_cast<double>(n) * sum_y2 - sum_y * sum_y;
    if (std::abs(denom_y) > 1e-12) {
        r2 = numerator * numerator / (denom_x * denom_y);
        // 防止浮点数误差
        r2 = std::clamp(r2, 0.0, 1.0);
    } else {
        r2 = 0.0;
    }

    return {k, b, r2, n};
}
