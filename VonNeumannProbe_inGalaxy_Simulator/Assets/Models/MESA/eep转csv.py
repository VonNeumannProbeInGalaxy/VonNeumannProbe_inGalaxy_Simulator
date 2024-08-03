import os
import csv

input_directory = "."  #当前目录
output_directory = "./1"  #输出目录

if not os.path.exists(output_directory):
    os.makedirs(output_directory)

columns_to_keep = ['star_age', 'star_mass','star_mdot', 'log_L', 'log_Teff', 'log_R', 'log_surf_z', 'v_wind_Km_per_s','log_center_T','log_center_Rho','phase']  #保留的列

with open("LIST.TXT", 'r') as list_file:
    files_to_process = [line.strip() for line in list_file]

def clean_output_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    cleaned_content = content.replace('.00000000e+00', '')

    cleaned_content = cleaned_content.replace('e+00', '')
    
    with open(file_path, 'w') as file:
        file.write(cleaned_content)

def convert_mass(filename):
    mass_str = filename.split('M')[0]
    mass = float(mass_str) / 100
    formatted_mass = f"{mass:.3f}"
    integer_part, fractional_part = formatted_mass.split('.')
    integer_part = integer_part.zfill(3)
    return f"{integer_part}.{fractional_part}"

def format_number(value):
    return f"{float(value):.8e}"

def process_file(file_path):
    with open(file_path, 'r') as f:
        lines = f.readlines()
    
    # 找到数据开始的行列
    start_line = 0
    column_indices = []
    for i, line in enumerate(lines):
        if line.startswith('#') and 'star_age' in line:
            start_line = i
            headers = line.strip('#').split()
            column_indices = [headers.index(col) for col in columns_to_keep]
            break

    base_name = os.path.basename(file_path)
    mass_part = convert_mass(base_name.split('M')[0])
    output_filename = os.path.join(output_directory, f"{mass_part}Ms_track.csv")
    
    with open(output_filename, 'w', newline='') as out_file:
        writer = csv.writer(out_file)
        writer.writerow(columns_to_keep)
        
        prev_age = None
        
        for line in lines[start_line+1:]:
            if not line.startswith('#'):
                data = line.split()
                formatted_data = [format_number(data[i]) for i in column_indices]
                
                current_age = formatted_data[0]
                
                if current_age != prev_age:
                    writer.writerow(formatted_data)
                    prev_age = current_age
    
    clean_output_file(output_filename)

for filename in files_to_process:
    file_path = os.path.join(input_directory, filename)
    if os.path.exists(file_path):
        process_file(file_path)