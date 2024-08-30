import csv
import os
from collections import defaultdict

star_types = {
    'M_V': ('M型红矮星', lambda x: x.startswith('M') and len(x) > 2 and x[2] == 'V'),
    'K_V': ('K型橙矮星', lambda x: x.startswith('K') and len(x) > 2 and x[2] == 'V'),
    'G_V': ('G型黄矮星', lambda x: x.startswith('G') and len(x) > 2 and x[2] == 'V'),
    'F_V': ('F型黄白色主序星', lambda x: x.startswith('F') and len(x) > 2 and x[2] == 'V'),
    'A_V': ('A型白主序星', lambda x: x.startswith('A') and len(x) > 2 and x[2] == 'V'),
    'B': ('B型蓝白色主序星', lambda x: x.startswith('B')),
    'O': ('O型蓝主序星', lambda x: x.startswith('O')),
    'W': ('沃尔夫拉叶星', lambda x: x.startswith('W')),
    'D': ('白矮星', lambda x: x.startswith('D')),
    'Q': ('中子星', lambda x: x.startswith('Q')),
    'X': ('黑洞', lambda x: x.startswith('X')),
    'Ia_Ib': ('超巨星', lambda x: x.endswith(('Ia', 'Ib', 'ab')) or (len(x) > 2 and x[2:].startswith(('Ia', 'Ib')))),
    'Ia+_0': ('特超巨星', lambda x: x.endswith('Ia+') or (len(x) > 2 and (x[2:].startswith('Ia+') or x[2] == '0')))
}

# 统计每种类型的恒星数量
star_counts = defaultdict(int)

brightest_giant = {'class': '', 'lum': float('-inf')}

brightest_blue = {'class': '', 'lum': float('-inf')}

# 获取脚本所在目录的相对路径
script_dir = os.path.dirname(__file__)
file_path = os.path.join(script_dir, 'Output.csv')

with open(file_path, 'r') as f:
    csv_reader = csv.reader(f)
    next(csv_reader)  # 跳过标题行
    for row in csv_reader:
        if len(row) >= 2:
            star_class, lum = row[0], row[1]
            try:
                lum_value = float(lum)
            except ValueError:
                continue

            is_giant = False
            is_blue = False

            # 检查1-10的分类
            for star_type, (chinese_name, condition) in list(star_types.items())[:11]:
                if condition(star_class):
                    star_counts[star_type] += 1
                    if chinese_name in ['O型蓝主序星', '沃尔夫拉叶星']:
                        is_blue = True
                        if lum_value > brightest_blue['lum']:
                            brightest_blue['class'] = star_class
                            brightest_blue['lum'] = lum_value
                    break  # 只能属于1-10中的一种

            # 检查11-12的分类
            for star_type, (chinese_name, condition) in list(star_types.items())[11:]:
                if condition(star_class):
                    star_counts[star_type] += 1
                    is_giant = True
                    if lum_value > brightest_giant['lum']:
                        brightest_giant['class'] = star_class
                        brightest_giant['lum'] = lum_value

            # 如果既是巨星又是蓝星，更新两个记录
            if is_giant and is_blue:
                if lum_value > brightest_giant['lum']:
                    brightest_giant['class'] = star_class
                    brightest_giant['lum'] = lum_value
                if lum_value > brightest_blue['lum']:
                    brightest_blue['class'] = star_class
                    brightest_blue['lum'] = lum_value

for star_type, (chinese_name, _) in star_types.items():
    print(f"{chinese_name}颗数：{star_counts[star_type]}")

print(f"\n光度最高的超巨星/特超巨星:")
print(f"Class: {brightest_giant['class']}")
print(f"Lum: {brightest_giant['lum']}")

print(f"\n光度最高的O型蓝主序星/沃尔夫拉叶星:")
print(f"Class: {brightest_blue['class']}")
print(f"Lum: {brightest_blue['lum']}")