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
#include <nlohmann/json.hpp>  // Include for JSON parsing

#define MAX_BUFFER 1024
using namespace std;
using json = nlohmann::json;
std::map<std::string, int> word_count;
json readConfig(const std::string& config_file_path) {
    std::ifstream config_file(config_file_path);

    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open " << config_file_path << " file!" << std::endl;
        exit(1);  // Exit if file cannot be opened
    }

    json config;
    config_file >> config;

    return config;  // Return the parsed JSON object
}


void clearBuffer(char* buffer, size_t size) {
    memset(buffer, 0, size);
}
std::stringstream stringToStream(const std::string& input) {
    std::stringstream ss(input);
    return ss;
}
auto Calculate_time(int sock, int p, int k, std::map<std::string, int>& word_count) {//second
    char buffer[MAX_BUFFER] = {0};
    bool end_of_file = false;
    int offset = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    while (!end_of_file) {
        // Client requests k words starting from the current offset
        std::string request = std::to_string(offset) + "\n";
        send(sock, request.c_str(), request.size(), 0);

        int wordsReceived = 0;

        // Read and process packets of p words until k words are received
        while (wordsReceived < k) {
            int valread = read(sock, buffer, MAX_BUFFER);
  
            std::stringstream ss = stringToStream(std::string(buffer, valread));
            std::string word;
            // Process each word in the packet
            while (std::getline(ss, word, '\n')) {
                if (word == "$$") {
                    end_of_file = true;
                    break;
                }
                else if (word == "EOF") {
                    end_of_file = true;
                    break;
                }
                word_count[word]=word_count[word]+1;
                wordsReceived=wordsReceived+1;
                if(false){

                }
                else if (wordsReceived >= k) {
                    break;
                }
            }

            if (end_of_file) {
                break;
            }
             clearBuffer(buffer, MAX_BUFFER);
            //memset(buffer, 0, MAX_BUFFER);  // Clear buffer for the next packet
        }

        // Update offset to request the next batch of k words
        offset =offset+ k;
       clearBuffer(buffer, MAX_BUFFER);
     
    }

    close(sock);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    return duration.count();
}

double runExperiment(const std::string& server_ip, int server_port, int p, int k, std::map<std::string, int>& word_count) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr); 
    auto check_connection=connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (check_connection < 0) 
    {
        std::cout << "Connection server-client failed" << std::endl;
        return -1;
    }


    auto temp=Calculate_time(sock,p,k,word_count);
    return temp;
}

int main() {
    json config = readConfig("config.json");
    std::string server_ip = config["server_ip"];
    int server_port = config["server_port"];
    int p = config["p"];
    int k = config["k"];
    int n = config["n"];
    std::string filename = config["filename"];

    

    // Run the experiment and track completion time
    double completion_time = runExperiment(server_ip, server_port, p, k, word_count);
    //freq
    std::ofstream wordCountFile("output.txt");

    if (!wordCountFile.is_open()) {
        std::cerr << "Error opening word_count.txt file!" << std::endl;
        return 1;
    }
    // Sort words alphabetically and write to file
    for (const auto& word_freq : word_count) {
        wordCountFile << word_freq.first << ", " << word_freq.second << "\n";
    }
    wordCountFile.close();

    std::ofstream outputFile("packetandtime.txt", std::ios_base::app);  // i'm appending to output.txt means overwriting on it
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output.txt file!" << std::endl;
        return 1;
    }
    outputFile << "p = " << p << ", completion_time = " << completion_time << " seconds\n";
    outputFile.close();

    std::cout << "Experiment complete. Results written to packetandtime.txt and output.txt" << std::endl;

    return 0;
}