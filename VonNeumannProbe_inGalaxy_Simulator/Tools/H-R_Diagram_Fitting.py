import os
import cv2
import numpy as np

# 获取当前脚本所在目录
script_directory = os.path.dirname(__file__)

# 构建相对路径
relative_path = os.path.join(script_directory, "H-R_Diagram_Curve.png")

# 读取图像
image = cv2.imread(relative_path)

# 检查图像是否成功读取
if image is None:
    print(f"无法读取图像文件: {relative_path}")
else:
    # 将图像转换为HSV颜色空间
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # 定义紫色的HSV范围
    lower_purple = np.array([125, 50, 50])
    upper_purple = np.array([150, 255, 255])

    # 使用颜色范围掩码提取紫色像素
    mask = cv2.inRange(hsv_image, lower_purple, upper_purple)

    # 获取紫色像素的坐标
    coordinates = np.column_stack(np.where(mask > 0))

    # 如果没有找到紫色像素，输出提示
    if coordinates.size == 0:
        print("没有找到紫色像素")
    else:
        # 输出所有紫色像素的坐标点
        print("紫色像素的坐标点:")
        for coord in coordinates:
            print(f"({coord[1]}, {coord[0]})")

    # 显示原图和掩码图像
    cv2.imshow('Original Image', image)
    cv2.imshow('Purple Mask', mask)
    cv2.waitKey(0)
    cv2.destroyAllWindows()