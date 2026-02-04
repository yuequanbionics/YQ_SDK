#ifndef TXRX_H_
#define TXRX_H_

#include "syst.hpp"
#include "CAN.hpp"

#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>

// script auto start -> IN_USE
// script auto end -> IN_USE

class GaussianFilter1D {
private:
    std::vector<double> kernel;         // 高斯核
    std::deque<double> buffer;          // 滑动窗口缓冲区
    std::vector<int> reflection_map;    // 镜像反射映射表
    int kernel_size;                    // 核大小
    int pad;                            // 边界填充大小
    int half_kernel;                    // 半核大小（用于对称性优化）
    bool use_reflection;                // 是否使用镜像反射

public:
    // 构造函数：初始化高斯核和反射映射表
    GaussianFilter1D(int kernel_size = 5, double sigma = 1.0, bool reflect = true)
        : kernel_size(kernel_size), use_reflection(reflect) {
        
        // 确保核大小为奇数
        if (kernel_size % 2 == 0) {
            kernel_size++;
            this->kernel_size = kernel_size;
        }
        
        pad = kernel_size / 2;
        half_kernel = pad;
        kernel = generate_gaussian_kernel(kernel_size, sigma);
        reflection_map.resize(2 * kernel_size - 1);
        
        // 预计算镜像反射映射表
        for (int i = 0; i < 2 * kernel_size - 1; ++i) {
            int mirror_idx = i;
            if (i < pad) {
                mirror_idx = pad - i;
            } else if (i >= kernel_size + pad) {
                mirror_idx = 3 * pad - (i - kernel_size);
            } else {
                mirror_idx = i - pad;
            }
            reflection_map[i] = mirror_idx;
        }
        
        // 初始化缓冲区
        reset();
    }

    // 生成一维高斯核（优化版本）
    std::vector<double> generate_gaussian_kernel(int size, double sigma) {
        std::vector<double> kernel(size, 0.0);
        int center = size / 2;
        double sum = 0.0;
        double sigma_sq = 2.0 * sigma * sigma;
        double denom = sqrt(2.0 * M_PI) * sigma;

        // 利用对称性，只计算一半
        for (int i = 0; i <= center; ++i) {
            double x = static_cast<double>(i);
            double val = exp(-(x * x) / sigma_sq) / denom;
            kernel[center + i] = val;
            if (i > 0) {
                kernel[center - i] = val;
            }
            sum += (i == 0) ? val : 2.0 * val;
        }

        // 归一化
        for (auto& val : kernel) {
            val /= sum;
        }

        return kernel;
    }

    // 优化后的滤波函数（减少条件判断和重复计算）
    double filter(double new_data) {
        // 将新数据添加到缓冲区尾部
        buffer.push_back(new_data);
        
        // 如果缓冲区大小超过核大小，移除最旧的数据
        if ((u32)buffer.size() > (u32)kernel_size) {
            buffer.pop_front();
        }

        // 应用高斯滤波（利用对称性优化计算）
        double result = 0.0;
        const int valid_size = buffer.size();
        const int buffer_offset = pad - (kernel_size - valid_size);
        
        // 中心点单独处理
        const int center_idx = buffer_offset + pad;
        if (center_idx >= 0 && center_idx < valid_size) {
            result += buffer[center_idx] * kernel[pad];
        } else if (use_reflection) {
            result += buffer[reflection_map[center_idx + pad]] * kernel[pad];
        }
        
        // 对称点对处理（减少一半乘法）
        for (int i = 1; i <= half_kernel; ++i) {
            const int left_idx = center_idx - i;
            const int right_idx = center_idx + i;
            
            double left_val = 0.0;
            double right_val = 0.0;
            
            if (left_idx >= 0 && left_idx < valid_size) {
                left_val = buffer[left_idx];
            } else if (use_reflection) {
                left_val = buffer[reflection_map[left_idx + pad]];
            }
            
            if (right_idx >= 0 && right_idx < valid_size) {
                right_val = buffer[right_idx];
            } else if (use_reflection) {
                right_val = buffer[reflection_map[right_idx + pad]];
            }
            
            // 利用对称性，k[pad-i] == k[pad+i]
            result += (left_val + right_val) * kernel[pad + i];
        }

        return result;
    }

    // 重置滤波器状态
    void reset() {
        buffer.clear();
        for (int i = 0; i < kernel_size - 1; ++i) {
            buffer.push_back(0.0);
        }
    }
};

#endif
