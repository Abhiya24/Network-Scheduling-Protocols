import subprocess
import json
import time

# Path to your JSON file, server.cpp, and client.cpp
json_file_path = 'config.json'
server_cpp = 'server.cpp'  # Path to server.cpp
client_cpp = 'client.cpp'  # Path to client.cpp
server_exec = './server'   # Path to compiled server executable
client_exec = './client'   # Path to compiled client executable

# The values for num_clients and initial port
num_clients_values = [1, 5, 9, 13, 17, 21, 25, 29]
initial_port = 8080

# Function to update the JSON file with num_clients and server_port
def update_json(num_clients, port):
    with open(json_file_path, 'r') as file:
        config = json.load(file)

    config['num_clients'] = num_clients
    config['server_port'] = port

    with open(json_file_path, 'w') as file:
        json.dump(config, file, indent=4)

# Function to compile server.cpp and client.cpp
def compile_cpp():
    subprocess.run(['g++', server_cpp, '-o', 'server'], check=True)
    subprocess.run(['g++', client_cpp, '-o', 'client'], check=True)

# Function to run the server and client executables
def run_server_and_client():
    server_process = subprocess.Popen([server_exec])  # Start the server
    time.sleep(2)  # Wait for the server to start

    # Run clients in parallel or sequentially as needed
    client_process = subprocess.Popen([client_exec])
    
    # Wait for both server and client processes to finish
    client_process.wait()
    server_process.terminate()  # Stop the server

# Main loop
for i, num_clients in enumerate(num_clients_values):
    port = initial_port + i
    print(f"Running with num_clients = {num_clients}, server_port = {port}")

    # Update the JSON file
    update_json(num_clients, port)

    # Compile server and client after updating the JSON
    compile_cpp()

    # Run server and client
    run_server_and_client()

    print(f"Completed run with num_clients = {num_clients}, server_port = {port}\n")
