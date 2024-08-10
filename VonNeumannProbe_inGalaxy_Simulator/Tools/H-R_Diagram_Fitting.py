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

    # 定义每条曲线的颜色范围（假设颜色范围已知）
    color_ranges = {
        'Ia': ([85, 100, 100], [95, 255, 255]),   # 青色
        'Ib': ([170, 100, 100], [180, 255, 255]), # 红色
        'II': ([50, 100, 100], [70, 255, 255]),   # 绿色
        'III': ([25, 100, 100], [35, 255, 255]),  # 黄色
        'IV': ([125, 100, 100], [150, 255, 255])  # 紫色
    }

    # 图像尺寸
    img_height, img_width = image.shape[:2]

    # 转换像素坐标为实际坐标的函数
    def pixel_to_actual(x, y):
        b_v = -0.3 + (x / 22.0) * 0.1
        luminosity = 10 ** (-5 + (img_height - y) / 63.4)
        return b_v, luminosity

    # 遍历每条曲线，提取坐标点并输出到文件
    for curve, (lower, upper) in color_ranges.items():
        lower = np.array(lower, dtype="uint8")
        upper = np.array(upper, dtype="uint8")
        
        # 创建掩膜
        mask = cv2.inRange(hsv_image, lower, upper)
        coordinates = []

        # 遍历图像的每一列，每隔5个像素取一个点
        for x in range(0, img_width, 5):
            for y in range(img_height):
                if mask[y, x] > 0:
                    coordinates.append((y, x))
                    break  # 确保同一列只取一个点

        if not coordinates:
            print(f"没有找到{curve}曲线的像素")
        else:
            output_file_path = os.path.join(script_directory, f"{curve}_coordinates.txt")
            print(f"{curve}曲线的坐标点已输出到文件: {output_file_path}")
            with open(output_file_path, "w") as file:
                for coord in coordinates:
                    b_v, luminosity = pixel_to_actual(coord[1], coord[0])
                    file.write(f"{b_v:.2f}, {luminosity:.5f}\n")

            # 显示对应的掩码图像
            cv2.imshow(f'{curve} Mask', mask)

    # 显示原图
    cv2.imshow('Original Image', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()