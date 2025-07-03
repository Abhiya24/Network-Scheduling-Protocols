import matplotlib.pyplot as plt
import re
from collections import defaultdict

def parse_output_file(file_path):
    p_times = defaultdict(list)

    # Regular expression to extract p values and completion times
    pattern = re.compile(r'p = (\d+), completion_time = ([\d\.]+) seconds')

    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                p = int(match.group(1))
                completion_time = float(match.group(2))
                p_times[p].append(completion_time)
    
    # Compute average completion time for each p value
    avg_p_values = []
    avg_completion_times = []
    for p, times in sorted(p_times.items()):
        avg_p_values.append(p)
        avg_completion_times.append(sum(times) / len(times))
    
    return avg_p_values, avg_completion_times

def plot_data(p_values, avg_completion_times, output_filename):
    plt.figure(figsize=(10, 6))
    plt.plot(p_values, avg_completion_times, marker='o', color='blue', linestyle='-', alpha=0.7)
    plt.xlabel('Number of Words per Packet (p)')
    plt.ylabel('Average Completion Time (seconds)')
    plt.title('Average Completion Time vs Number of Words per Packet')
    plt.grid(True)
    
    # Save the plot as a file instead of showing it
    plt.savefig(output_filename)

def main():
    file_path = 'packetandtime.txt'
    output_filename = 'completion_time_plot.png'
    
    p_values, avg_completion_times = parse_output_file(file_path)
    plot_data(p_values, avg_completion_times, output_filename)

if __name__ == "__main__":
    main()
