# Multi-Client Word Counting over TCP

This module extends the basic TCP client-server model to support **multiple concurrent clients**. The goal is to evaluate the performance and scalability of the server as the number of clients increases.

---

##  Functionality

- Multiple clients simultaneously request words from a common file hosted by the server.
- The server handles each connection using POSIX threads.
- Clients fetch blocks of words and calculate frequency locally.
- Performance is measured in terms of **average completion time per client**.

---

##  Experiment: Scaling with Concurrent Clients

### Goal:
Evaluate how completion time per client changes as number of clients increases from **1 to 29**.

### Configuration:
- `p = 10`: number of words per packet
- `k = 5`: number of words requested in total per client
- Clients run on the same machine

### Observed Behavior:
- Time improves initially with more clients (better resource use)
- Stabilizes between 5–20 clients
- Increases again beyond 20 clients due to resource contention

 Plot generated:  
![Completion Time vs Clients](./plot.png)

---

##  Files

- `client.cpp` – Multi-threaded TCP client
- `server.cpp` – Multi-threaded TCP server
- `config.json` – Contains `server_ip`, `server_port`, `num_clients`, `k`, `p`
- `demo.py` – Automates experiments with increasing client count
- `Makefile` – Compiles and runs everything using `make`
- `plotcount.py` – Python script to generate performance plot
- `plot.png` – Final graph (completion time vs. number of clients)
- `testcase.txt` – Word list used as input

---

##  demo.py – Experiment Automation

The script runs the full experiment by:
- Updating `config.json` with a new `num_clients` and port
- Compiling client and server
- Running server and client, collecting performance data

### Usage:
```bash
python3 demo.py

### How to Run:
-Compile:
 make build
-Run experiment:
 make run     # runs demo.py
-Generate Plot:
 make plot
 Make sure all dependencies (e.g., matplotlib, json) are installed.

### Notes
Written in C++ using POSIX sockets and threading

Tested on local machine (Linux/macOS)

Part of Computer Networks @ IIT Delhi
