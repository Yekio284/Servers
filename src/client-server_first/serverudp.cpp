#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <iostream>

#define BUFLEN 81

int main() {
    int sockMain, length, msgLength;
    struct sockaddr_in servAddr, clientAddr;
    char buf[BUFLEN];

    if ((sockMain = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Server can't open the socket for UDP.");
        return -1;
    }

    bzero((char *) &servAddr, sizeof(servAddr));
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;

    if (bind(sockMain, (struct sockaddr *) &servAddr, sizeof(servAddr))) {
        perror("Failed binding the server.");
        return -2;
    }

    length = sizeof(servAddr);
    if (getsockname(sockMain, (struct sockaddr *) &servAddr, (socklen_t *) &length)) {
        perror("Failed to call getsockname.");
        return -3;
    }

    std::cout << "SERVER: number of port - " << ntohs(servAddr.sin_port) << std::endl;
    std::cout << "Waiting for messages..." << std::endl << std::endl;

    while (true) {
        length = sizeof(clientAddr);
        bzero(buf, sizeof(BUFLEN));
        
        if ((msgLength = recvfrom(sockMain, buf, BUFLEN, 0, (struct sockaddr *) &clientAddr, (socklen_t *) &length)) < 0) {
            perror("Bad client socket.");
            return -4;
        }

        std::cout << "SERVER: IP address of the client: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
        std::cout << "SERVER: PORT of the client: " << ntohs(clientAddr.sin_port) << std::endl;
        std::cout << "SERVER: MSG LENGTH: " << msgLength << std::endl;
        std::cout << "SERVER: MSG: ";
        for (int i = 0; i < msgLength; i++) {
            std::cout << buf[i];
        }
        std::cout << std::endl << std::endl;
    }
    
    return 0;
}