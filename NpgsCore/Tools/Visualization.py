import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os

# 获取当前脚本所在目录
current_dir = os.path.dirname(__file__)

# 构建数据文件的相对路径
file_path = os.path.join(current_dir, 'Output.csv')

# 读取数据文件
data = pd.read_csv(file_path, header=None, names=['X', 'Y', 'Z'])

# 提取X、Y、Z列数据
x = data['X']
y = data['Y']
z = data['Z']

# 创建3D图形
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 绘制散点图
ax.scatter(x, y, z, c='r', marker='o')

# 设置坐标轴标签
ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

# 设置坐标轴范围
ax.set_xlim([x.min(), x.max()])
ax.set_ylim([y.min(), y.max()])
ax.set_zlim([z.min(), z.max()])

# 显示图形
plt.show()