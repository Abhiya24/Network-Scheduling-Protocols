#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <nlohmann/json.hpp>  // Include for JSON parsing
#define MAX_BUFFER 1024
using namespace std;
using json = nlohmann::json;

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
void serveClient(int client_socket, std::vector<std::string>& words, int p, int k, int ssize) {
    int size=ssize;
    char buffer[size] = {0};
    int valread;

    while ((valread = read(client_socket, buffer, MAX_BUFFER)) > 0) {
        std::stringstream ss = stringToStream(std::string(buffer, valread));
        int offset;
        ss >> offset;

        // Check if the offset exceeds the number of words
        if(false){
            cout<<"not working"<<endl;
        }
        else if(offset >= words.size()) {
            std::string response = "$$\n";  // Signal that offset is out of range
            send(client_socket, response.c_str(), response.size(), 0);
            continue;
        }
        std::vector<std::string> responseWords;
        for (int i = 0; i < k && (offset + i) < words.size(); ++i) {

            responseWords.push_back(words[offset + i]);
        }



        // Send the `k` words in packets, each with `p` words
        int wordsSent = 0;
        while (!(wordsSent >= responseWords.size())) {
            std::string responseChunk;

            // Collect `p` words for the current packet
            for (int i = 0; i < p && (wordsSent + i) < responseWords.size(); ++i) {
             for(int j=0 ; j<p ; j++)
                 {        packetcount(p);   
                     }
                responseChunk += responseWords[wordsSent + i] + "\n";
            }

            // Send the current packet to the client
            send(client_socket, responseChunk.c_str(), responseChunk.size(), 0);
            wordsSent =wordsSent+ p; 
        }

        // Signal EOF if we reach the end of the words
        if(false){
            cout<<"not chcecked";
        }
        else if (offset + k >= words.size()) {
            std::string eofMessage = "EOF\n";
            send(client_socket, eofMessage.c_str(), eofMessage.size(), 0);
        }

        clearBuffer(buffer, MAX_BUFFER);  // Clear buffer for the next request
    }

    close(client_socket);
}

int main() {
    
    json config = readConfig("config.json");
    std::string server_ip = config["server_ip"];
    int PORT = config["server_port"];
    int p = config["p"];
    int k = config["k"];
    int n = config["n"];
    std::string filename = config["filename"];
//cretaing the socket
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); 
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, n);

    std::vector<std::string> words = readWordsFromFile(filename);//i'm reading file



    while (true) {
            client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
      
        if ((client_socket) >= 0) {
        std::cout << "Connection is established with client" << std::endl;
        serveClient(client_socket, words,p,k,MAX_BUFFER);}

        else if ((client_socket) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
