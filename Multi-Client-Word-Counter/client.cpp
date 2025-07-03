#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <pthread.h>  
#include <nlohmann/json.hpp>

#define MAX_BUFFER 1024
using namespace std;
using json = nlohmann::json;

//client ka structure
struct ClientParams {
    std::string server_ip;
    int server_port;
    int p;
    std::map<std::string, int> word_count;
    double completion_time;
};

json readConfig(const std::string& filename) {//1
    std::ifstream config_file(filename);

    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open " << filename << " file!" << std::endl;
        exit(1);  // Exit if the file can't be opened
    }

    json config;
    config_file >> config;
    return config;
}

void clearBuffer(char* buffer, size_t size) {
    memset(buffer, 0, size);
}

// Function to run the experiment in a separate thread
void* clientThread(void* args) {
    ClientParams* params = (ClientParams*)args;
    
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(params->server_port);

    int ip_checkkk=inet_pton(AF_INET, params->server_ip.c_str(), &serv_addr.sin_addr);

    int connection_checkkk = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    if ( ip_checkkk <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        pthread_exit(NULL); }
    else if ( connection_checkkk< 0) {
        std::cerr << "Connection failed" << std::endl;
        pthread_exit(NULL); }

    int offset = 0;
    bool end_of_file = false;

    auto start_time = std::chrono::high_resolution_clock::now();//clock startttttttt

    while (!end_of_file) {
        
        std::string request = std::to_string(offset) + "\n";
        send(sock, request.c_str(), request.size(), 0);

        int valread = read(sock, buffer, MAX_BUFFER);
        std::string response(buffer, valread);
        std::stringstream ss(response);
        std::string line;

        while (std::getline(ss, line)) {
            if (line == "EOF") {
                end_of_file = true;
                break;
            }
            else if (line == "$$") {
                end_of_file = true;
                break;
            }

            std::stringstream word_stream(line);
            std::string word;
            while (std::getline(word_stream, word, ',')) {
                // Increment word count in the map
                params->word_count[word]= params->word_count[word]+1;
            }
        }

  
        offset = offset+params->p;

        clearBuffer(buffer, MAX_BUFFER);   // Clear buffer for next iteration
    }

    close(sock);

    
    auto end_time = std::chrono::high_resolution_clock::now();//clock endddddddddd
    std::chrono::duration<double> duration = end_time - start_time;
    
    // Store the elapsed time for the experiment
    params->completion_time = duration.count();

    pthread_exit(NULL);
}

int main() {


    json config = readConfig("config.json");
    std::string server_ip = config["server_ip"];
    int server_port = config["server_port"];
    int p = config["p"];
    int k = config["k"];
    std::string input_file = config["input_file"];
    int num_clients= config["num_clients"];
 

    pthread_t threads[num_clients];

    ClientParams client_params[num_clients];  

    for (int i = 0; i < num_clients; i++) {
        client_params[i].server_ip = server_ip;
        client_params[i].server_port = server_port;
        client_params[i].p = p;

        if (pthread_create(&threads[i], NULL, clientThread, &client_params[i]) != 0) {
            std::cerr << "Error creating thread " << i + 1 << std::endl;
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    
    std::ofstream outputFile("avgtimes.txt", std::ios_base::app);  // Appending to output.txt

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output.txt file!" << std::endl;
        return 1;
    }

    // Log average completion time to output.txt
    double total_time = 0;
    for (int i = 0; i < num_clients; i++) {
        total_time += client_params[i].completion_time;
    }
    double avg_time = total_time / num_clients;

    outputFile << "p = " << p << ", num_clients = " << num_clients << ", average_completion_time = " << avg_time << " seconds\n";

    outputFile.close();
//now store words with freq
std::ofstream wordcount_file("output.txt");

if (!wordcount_file.is_open()) {
    std::cerr << "Error opening counting.txt file!" << std::endl;
    return 1;
}

// Create a sorted vector of words
std::vector<std::pair<std::string, int>> sorted_word_count(client_params[0].word_count.begin(), client_params[0].word_count.end());
std::sort(sorted_word_count.begin(), sorted_word_count.end());


for (const auto& pair : sorted_word_count) {

    wordcount_file << pair.first << ", " << pair.second << "\n";
}

wordcount_file.close();

    std::cout << "Experiment complete. Results written to output.txt & avgtime" << std::endl;

    return 0;
}




