# Word Counting TCP Client-Server

This project implements a TCP-based client-server system where a client requests words from a file hosted on the server, receives them in chunks, and computes the frequency of each word.

---

##  Functionality

- The **client** requests words starting from a given offset.
- The **server** responds with `k` words in chunks of size `p`.
- This continues until the server sends the special word `EOF`.
- Finally, the client outputs word frequencies in **dictionary order**.

---

##  Communication Protocol

### Client sends:
- `offset\n` → to request words from that position.

### Server responds with:
- `word_1\nword_2\n...word_k\n`
- or `$$\n` if offset > word list size.

---

##  Experiment

Completion time was measured for different values of `p` (words per packet), ranging from 1 to 10, and plotted.

 See `plot.png` and `report.pdf` in the root folder for the analysis and observations.

---

## ️ Files

- `client.cpp` – Client-side socket logic
- `server.cpp` – Server-side logic with memory-mapped word file
- `demo.py` – Automates experiments by varying packet size (`p`) and runs multiple iterations
- `Makefile` – Build + run system
- `config.json` – Contains server IP, port, k, p, filename, and number of clients
- `plot.png` – Graph of completion time vs. p

---

##  How to Run
```bash
make build       # Compiles both server and client
make run         # Runs the client-server system using config.json
make plot        # Generates the plot for completion times

