import json
import os
import time
import subprocess

# Path to config file
config_path = "config.json"

# Number of iterations
iterations = 10

def update_config(p):
    # Read the existing config file
    with open(config_path, "r") as config_file:
        config = json.load(config_file)

    # Update the value of 'p'
    config["p"] = p

    # Write the updated config back to the file
    with open(config_path, "w") as config_file:
        json.dump(config, config_file, indent=4)

def run_client_server(p, iteration):
    # Start the server in a separate process
    server_process = subprocess.Popen(["./server"])

    # Give the server time to start up
    time.sleep(1)

    subprocess.run(["./client"])

    # Stop the server
    server_process.terminate()
    server_process.wait()


for p in range(1, 11):
    print(f"Running experiment for p = {p}")

    update_config(p)


    for i in range(iterations):
        print(f"  Iteration {i+1} for p = {p}")
        run_client_server(p, i+1)

    print(f"Completed experiments for p = {p}")

print("All experiments completed.")
