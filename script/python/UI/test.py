import matplotlib.pyplot as plt
import numpy as np  # 新增：用于线性拟合计算

# 整理数据
x = [0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20]
y = [2, 4, 5, 6, 8, 9, 10, 12, 14, 14, 15, 17, 18, 18, 20, 21, 21]

# ========== 核心：线性拟合计算 ==========
# 用np.polyfit做一次多项式（线性）拟合，返回 [斜率, 截距]
slope, intercept = np.polyfit(x, y, 1)
# 生成拟合线的y值（基于原x坐标）
y_fit = slope * np.array(x) + intercept
# 打印拟合公式（方便参考）
print(f"线性拟合公式：y = {slope:.2f}x + {intercept:.2f}")

# 创建画布
plt.figure(figsize=(10, 6))

# 绘制原始数据：散点图+趋势线
plt.scatter(x, y, color='red', s=50, label='Data Points')
plt.plot(x, y, color='blue', linestyle='-', linewidth=1.5, label='Original Trend Line')

# 绘制线性拟合参考线（用虚线区分，便于识别）
plt.plot(x, y_fit, color='green', linestyle='--', linewidth=2, label=f'Linear Fit (y={slope:.2f}x+{intercept:.2f})')

# 添加标题和坐标轴标签
plt.title('x-y Coordinate Data Visualization with Linear Fit', fontsize=14)
plt.xlabel('X Coordinate', fontsize=12)
plt.ylabel('Y Coordinate', fontsize=12)

# 网格和图例
plt.grid(True, alpha=0.3)
plt.legend()

# 显示图表
plt.show()
