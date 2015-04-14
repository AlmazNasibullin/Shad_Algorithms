#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <algorithm>

std::string getHostname(std::string &msg) {
    std::string pattern = "Host: ";
    std::string::size_type startPosition = msg.find(pattern);
    if (startPosition == std::string::npos) {
        throw std::runtime_error("Hostname wasn't found");
    }
    startPosition += pattern.size();
    std::string::size_type endPosition =
        std::min(msg.find(" ", startPosition), 
            std::min(msg.find("\r", startPosition),
            msg.find("\n", startPosition)));
    return msg.substr(startPosition, endPosition - startPosition);
}

std::string makeGetRequets(std::string &msg,
                            std::stringstream &log,
                            std::mutex &m,
                            char* IP) {
    if (msg.substr(0, 3).compare("GET") != 0) {
        throw std::runtime_error("No get request");
    }
    std::string hostname = getHostname(msg);

    {
        std::lock_guard<std::mutex> lock(m);
        log << IP << " " << hostname << "\n";
    }

    int sckt = socket(AF_INET, SOCK_STREAM, 0);
    if (sckt < 0) {
        throw std::runtime_error("Getter: socket error");
    }

    struct sockaddr_in addr = {AF_INET, htons(80)};
    struct hostent* h = gethostbyname(&hostname[0]);
    if (!h) {
        close(sckt);
        throw std::runtime_error("Getter: gethostbyname error");
    }
    int res = inet_aton(inet_ntoa(*((struct in_addr*)h->h_addr)), &addr.sin_addr);
    if (res < 0 || !res) {
        close(sckt);
        throw std::runtime_error("Getter: inet_aton error");   
    }

    int cnt = connect(sckt, (struct sockaddr*)&addr, sizeof(addr));
    if (cnt < 0) {
        close(sckt);
        throw std::runtime_error("Getter: connect error");
    }
    
    send(sckt, &msg[0], msg.size(),0);

    char buf[1024];
    std::stringstream ss;
    while (1) {
        int reads = recv(sckt, buf, 1024, 0);
        if (reads <= 0)
            break;
        ss << std::string(buf, reads);
    }
    close(sckt);
    return ss.str();
}

void runGetter(int sock, std::stringstream &log, std::mutex &m, char* IP) {
    char buf[1024];
    int reads = recv(sock, buf, 1024, 0);
    std::string msg(buf, reads);
    
    try {
        std::string answer = makeGetRequets(msg, log, m, IP);
        send(sock, &answer[0], answer.size(), 0);
    } catch (std::runtime_error re) {
        if (std::strcmp(re.what(),
                "Getter: gethostbyname error") == 0) {
            std::string answer = "Invalid hostname";
            send(sock, &answer[0], answer.size(), 0);
        } else if (std::strcmp(re.what(), "No get request") == 0) {
            // not get request
        } else {
            std::string answer = re.what();
            send(sock, &answer[0], answer.size(), 0);
        }
    } catch (...) {
        std::string answer = "Error happened";
        send(sock, &answer[0], answer.size(), 0);
    }

    close(sock);
}

void writeLogToFile(std::stringstream &log, std::mutex& m) {
    std::ofstream logFile;
    try {
        logFile.open ("LogFile");
        {
            std::lock_guard<std::mutex> lock(m);
            logFile << log.str();
            log.clear();
        }
        logFile.close();
    } catch (...) {
        logFile.close();
    }
}

void runListener(int port) {
    int sock, listener;
    struct sockaddr_in addr;
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        std::cout << "Server: socket error" << std::endl;
        exit(1);
    }
    std::cout << "Server: socket opened\n";

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Server: binding error" << std::endl;
        close(listener);
        exit(1);
    }
    std::cout << "Server: binding finished\n";
    
    listen(listener, 1);
    
    std::mutex m;
    std::stringstream log;
    int requestCount = 0;

    std::cout << "Server: listening started on port " << port << "\n";

    while (1) {
        struct sockaddr_in clientAddr;
        unsigned int clientAddrLen = sizeof(clientAddr);
        sock = accept(listener, (struct sockaddr*)&clientAddr,
            &clientAddrLen);
        if (sock < 0) {
            std::cout << "Server: accepting error" << std::endl;
            continue;
        }
        
        std::thread getter(runGetter, sock, std::ref(log), std::ref(m),
            inet_ntoa(clientAddr.sin_addr));
        getter.detach();
        
        ++requestCount;
        if (requestCount == 50) {
            writeLogToFile(log, m);
            requestCount = 0;
        }
    }

    if (requestCount > 0) {
        writeLogToFile(log, m);
    }
    close(listener);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Should be one argument: port\n";
        return 0;
    }
    const int port = std::stoi(argv[1]);
    runListener(port);
    return 0;
}
