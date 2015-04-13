#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>

void runClient(int port, char* hostname) {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "Client: socket error" << std::endl;
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Client: connection error" << std::endl;
        close(sock);
        exit(1);
    }
    
    send(sock, hostname, std::strlen(hostname), 0);

    std::stringstream data;
    char buf[1024];
    while (1) {
        int reads = recv(sock, buf, 1024, 0);
        if (reads <= 0) {
            break;
        }
        data << std::string(buf, reads);
    }

    std::cout << "----------------------------------\n";
    std::cout << data.str() << std::endl;
    std::cout << "----------------------------------\n";
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Should be two argument: port, hostname\n";
        return 0;
    }
    const int port = std::stoi(argv[1]);
    runClient(port, argv[2]);

    return 0;
}
