import matplotlib.pyplot as plt

# Initialize lists to store number of clients and corresponding average completion times
num_clients = []
avg_completion_times = []

# Read the output.txt file and extract the number of clients and average completion times
with open("avgtimes.txt", "r") as file:
    for line in file:
        if "num_clients" in line and "average_completion_time" in line:
            # Split the line by commas and extract relevant parts
            parts = line.strip().split(",")
            
            # Extract number of clients
            num_clients_value = int(parts[1].split('=')[1].strip())
            num_clients.append(num_clients_value)
            
            # Extract average completion time
            avg_completion_time_value = float(parts[2].split('=')[1].strip().split()[0])
            avg_completion_times.append(avg_completion_time_value)

# Plot the results
plt.plot(num_clients, avg_completion_times, marker='o')
plt.title('Average Completion Time vs Number of Clients')
plt.xlabel('Number of Clients')
plt.ylabel('Average Completion Time (seconds)')
plt.grid(True)

# Save the plot as plot.png
plt.savefig('plot.png')

# plt.show() is optional; if you're running in a non-GUI environment, remove it or comment it out.
# plt.show() 
