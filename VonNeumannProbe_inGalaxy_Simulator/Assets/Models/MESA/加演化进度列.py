import os
import csv

def calculate_x(current_time, current_phase, phase_start_time, phase_end_time):
    if phase_start_time == phase_end_time:
        return float(current_phase)
    progress = (current_time - phase_start_time) / (phase_end_time - phase_start_time)
    return float(current_phase) + progress

def process_csv(input_file, output_file):
    with open(input_file, 'r') as f:
        reader = csv.reader(f)
        headers = next(reader)
        data = list(reader)

    phase_times = {}
    current_phase = None
    for row in data:
        time = float(row[0])
        phase = int(row[-1])
        
        if phase != current_phase:
            if current_phase is not None:
                phase_times[current_phase][1] = time
            phase_times[phase] = [time, None]
            current_phase = phase

    phase_times[current_phase][1] = float(data[-1][0])

    processed_data = []
    for row in data:
        time = float(row[0])
        phase = int(row[-1])
        phase_start = phase_times[phase][0]
        phase_end = phase_times[phase][1]
        
        x = calculate_x(time, phase, phase_start, phase_end)
        processed_row = row + [x]
        processed_data.append(processed_row)

    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers + ['x'])
        writer.writerows(processed_data)

def main():
    output_dir = '1'
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for filename in os.listdir('.'):
        if filename.endswith('.csv'):
            input_file = filename
            output_file = os.path.join(output_dir, filename)
            process_csv(input_file, output_file)

if __name__ == "__main__":
    main()