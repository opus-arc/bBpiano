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


LinearRegressionResult LinearRegression::fit(const std::vector<std::array<double, 2>> &points) {
    
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


QuadraticRegressionResult LinearRegression::fit2(const std::vector<std::array<float, 2>> &points) {

    const std::size_t n = points.size();

    if (n < 3)
        throw std::invalid_argument("二次回归至少需要三个点吧");

    double sum_x = 0.0;
    double sum_x2 = 0.0;
    double sum_x3 = 0.0;
    double sum_x4 = 0.0;

    double sum_y = 0.0;
    double sum_xy = 0.0;
    double sum_x2y = 0.0;

    for (const auto& p : points) {

        const double x = static_cast<double>(p[0]);
        const double y = static_cast<double>(p[1]);

        const double x2 = x * x;

        sum_x += x;
        sum_x2 += x2;
        sum_x3 += x2 * x;
        sum_x4 += x2 * x2;

        sum_y += y;
        sum_xy += x * y;
        sum_x2y += x2 * y;
    }

    double matrix[3][4] = {
        {sum_x4, sum_x3, sum_x2, sum_x2y},
        {sum_x3, sum_x2, sum_x,  sum_xy},
        {sum_x2, sum_x,  static_cast<double>(n), sum_y}
    };

    for (int col = 0; col < 3; ++col) {

        int pivot = col;

        for (int row = col + 1; row < 3; ++row) {
            if (std::abs(matrix[row][col]) > std::abs(matrix[pivot][col])) {
                pivot = row;
            }
        }

        if (std::abs(matrix[pivot][col]) < 1e-12)
            throw std::runtime_error("不能回归出结果，因为 x 太接近了");

        if (pivot != col) {
            for (int k = 0; k < 4; ++k) {
                std::swap(matrix[col][k], matrix[pivot][k]);
            }
        }

        const double divisor = matrix[col][col];

        for (int k = 0; k < 4; ++k) {
            matrix[col][k] /= divisor;
        }

        for (int row = 0; row < 3; ++row) {

            if (row == col)
                continue;

            const double factor = matrix[row][col];

            for (int k = 0; k < 4; ++k) {
                matrix[row][k] -= factor * matrix[col][k];
            }
        }
    }

    const double a = matrix[0][3];
    const double b = matrix[1][3];
    const double c = matrix[2][3];

    const double mean_y = sum_y / static_cast<double>(n);

    double ss_res = 0.0;
    double ss_tot = 0.0;

    for (const auto& p : points) {

        const double x = static_cast<double>(p[0]);
        const double y = static_cast<double>(p[1]);

        const double predicted_y = a * x * x + b * x + c;

        const double residual = y - predicted_y;
        const double centered_y = y - mean_y;

        ss_res += residual * residual;
        ss_tot += centered_y * centered_y;
    }

    double r2 = 0.0;

    if (std::abs(ss_tot) > 1e-12) {
        r2 = 1.0 - ss_res / ss_tot;
        r2 = std::clamp(r2, 0.0, 1.0);
    }

    return {a, b, c, r2, n};
}

QuadraticRegressionResult LinearRegression::fit2(const std::vector<std::array<double, 2>> &points) {

    const std::size_t n = points.size();

    if (n < 3)
        throw std::invalid_argument("二次回归至少需要三个点吧");

    double sum_x = 0.0;
    double sum_x2 = 0.0;
    double sum_x3 = 0.0;
    double sum_x4 = 0.0;

    double sum_y = 0.0;
    double sum_xy = 0.0;
    double sum_x2y = 0.0;

    for (const auto& p : points) {

        const double x = p[0];
        const double y = p[1];

        const double x2 = x * x;

        sum_x += x;
        sum_x2 += x2;
        sum_x3 += x2 * x;
        sum_x4 += x2 * x2;

        sum_y += y;
        sum_xy += x * y;
        sum_x2y += x2 * y;
    }

    double matrix[3][4] = {
        {sum_x4, sum_x3, sum_x2, sum_x2y},
        {sum_x3, sum_x2, sum_x,  sum_xy},
        {sum_x2, sum_x,  static_cast<double>(n), sum_y}
    };

    for (int col = 0; col < 3; ++col) {

        int pivot = col;

        for (int row = col + 1; row < 3; ++row) {
            if (std::abs(matrix[row][col]) > std::abs(matrix[pivot][col])) {
                pivot = row;
            }
        }

        if (std::abs(matrix[pivot][col]) < 1e-12)
            throw std::runtime_error("不能回归出结果，因为 x 太接近了");

        if (pivot != col) {
            for (int k = 0; k < 4; ++k) {
                std::swap(matrix[col][k], matrix[pivot][k]);
            }
        }

        const double divisor = matrix[col][col];

        for (int k = 0; k < 4; ++k) {
            matrix[col][k] /= divisor;
        }

        for (int row = 0; row < 3; ++row) {

            if (row == col)
                continue;

            const double factor = matrix[row][col];

            for (int k = 0; k < 4; ++k) {
                matrix[row][k] -= factor * matrix[col][k];
            }
        }
    }

    const double a = matrix[0][3];
    const double b = matrix[1][3];
    const double c = matrix[2][3];

    const double mean_y = sum_y / static_cast<double>(n);

    double ss_res = 0.0;
    double ss_tot = 0.0;

    for (const auto& p : points) {

        const double x = p[0];
        const double y = p[1];

        const double predicted_y = a * x * x + b * x + c;

        const double residual = y - predicted_y;
        const double centered_y = y - mean_y;

        ss_res += residual * residual;
        ss_tot += centered_y * centered_y;
    }

    double r2 = 0.0;

    if (std::abs(ss_tot) > 1e-12) {
        r2 = 1.0 - ss_res / ss_tot;
        r2 = std::clamp(r2, 0.0, 1.0);
    }

    return {a, b, c, r2, n};
}
