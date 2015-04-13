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

std::string makeGetRequets(char *hostname) {
    int sckt = socket(AF_INET, SOCK_STREAM, 0);
    if (sckt < 0) {
        throw std::runtime_error("Getter: socket error");
    }

    struct sockaddr_in addr = {AF_INET, htons(80)};
    struct hostent* h = gethostbyname(hostname);
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
    
    std::string msg = "GET / HTTP/1.1\r\nHost: " + std::string(hostname)
        + "\r\nConnection: close\r\n\r\n";
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
    char buf[256];
    int count = recv(sock, buf, 256, 0);
    std::string hostname(buf, count);

    try {
        std::string answer = makeGetRequets(&hostname[0]);
        {
            std::lock_guard<std::mutex> lock(m);
            log << IP << " " << hostname << "\n";
        }
        send(sock, &answer[0], answer.size(), 0);
    } catch (std::runtime_error re) {
        if (std::strcmp(re.what(),
                "Getter: gethostbyname error") == 0) {
            std::string answer = "Invalid hostname";
            send(sock, &answer[0], answer.size(), 0);
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
