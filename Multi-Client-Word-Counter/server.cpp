#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <pthread.h>
#define MAX_CLIENTS 32
#define MAX_BUFFER 1024
using namespace std;
using json = nlohmann::json;

struct ClientHandlerParams {
    int client_socket;
    std::vector<std::string> words;
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
    std::string server_ip;
    int server_port;
    int p;
    int k;
    int num_clients;

std::vector<std::string> splitStringByDelimiter(const std::string& str, char delimiter = ',') {
    std::vector<std::string> wordsline;
    std::string word;
    std::stringstream s(str);

    while (std::getline(s, word, delimiter)) {
        wordsline.push_back(word);
    }

    return wordsline;
}
std::vector<std::string> spliting(const std::string& str,int a) {
    return splitStringByDelimiter(str, ',');  }


std::vector<std::string> readWordsFromFile(const std::string& filename) {//1
    std::ifstream file(filename); 
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        exit(1); 
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();  // Read entire file into buffer
    std::string fileContent = buffer.str();
    std::vector<std::string> words = spliting(fileContent,1);
    
    return words;
}


void packetcount(int a){
   int  count=a;
    count++;
}


void clearBuffer(char* buffer, size_t size) {
    memset(buffer, 0, size);
}

std::stringstream stringToStream(const std::string& input) {
    std::stringstream ss(input);
    return ss;
}


bool readFromSocket(int client_socket, char* buffer) {
    int valread = read(client_socket, buffer, MAX_BUFFER);
    if (valread <= 0) {
        return false;  // Error or client disconnected
    }
    return true;
}



void* handleClient(void* params) {
    ClientHandlerParams* chp = (ClientHandlerParams*)params;
    int client_socket = chp->client_socket;
    char buffer[MAX_BUFFER] = {0};
    bool end_of_file = false;



    while (!end_of_file) {
 
        if (!readFromSocket(client_socket, buffer)) {
        break;  // Error or client disconnected
    }

        std::stringstream ss(buffer);
        int offset;
        ss >> offset;

        std::string response;

        switch (offset >= chp->words.size()) {
    case true:
        response = "$$\n";  // If offset exceeds available words, send "$$"
        break;
    
    case false:
        int count = 0;
        for (int i = offset; i < chp->words.size() && count < p; ++i, ++count) {
            response += chp->words[i] + "\n";
        }
        
        switch (offset + p >= chp->words.size()) {
            case true:
                response += "EOF\n";  
                end_of_file = true;
                break;
            case false:
               
                break;
        }
        break;
}


    send(client_socket, response.c_str(), response.size(), 0);
      clearBuffer(buffer, MAX_BUFFER);   // Clear buffer for next iteration
    }

    close(client_socket);
    pthread_exit(nullptr);
}

int main() {
    json config = readConfig("config.json");
    server_ip = config["server_ip"];
    server_port = config["server_port"];
    p = config["p"];
    k = config["k"];
    std::string input_file = config["input_file"];
    num_clients= config["num_clients"];

    // Load words from file
    std::ifstream word_file(input_file);
    if (!word_file.is_open()) {
        std::cerr << "Error: Could not open words file!" << std::endl;
        return 1;
    }

    std::vector<std::string> words;
    std::string word;
    while (std::getline(word_file, word)) {
        words.push_back(word);
    }

    // Set up server socket
    int serverX, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    serverX = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    if (bind(serverX, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(serverX, num_clients) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << server_port << std::endl;

    pthread_t client_threads[MAX_CLIENTS];
    int client_count = 0;

    while (client_count < num_clients && (new_socket = accept(serverX, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
        ClientHandlerParams* params = new ClientHandlerParams{new_socket, words};
        pthread_create(&client_threads[client_count], nullptr, handleClient, (void*)params);
        client_count++;
    }

    for (int i = 0; i < client_count; ++i) {
        pthread_join(client_threads[i], nullptr);
    }

    close(serverX);
    return 0;
}
