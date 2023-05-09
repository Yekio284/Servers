#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
    int mainSock;
    struct sockaddr_in servAddr;
    
    if (argc != 4) {
        std::cerr << "Incorrect input. Example: " << argv[0] << " <IP> <PORT> <i>" << std::endl;
        exit(EXIT_FAILURE);
    }

    if ((mainSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error with socket() " << std::endl;
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(mainSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
        std::cerr << "Error with connect() " << std::endl;
        exit(EXIT_FAILURE);
    }

    int i = atoi(argv[3]);
    for (int j = 0; j < i; j++) {
        if (send(mainSock, argv[3], strlen(argv[3]), 0) == -1) {
       		std::cerr << "Error with send() " << std::endl;
       		exit(EXIT_FAILURE);
    	}
        std::cout << "Send to server: " << i << std::endl;
        
        sleep(i);
    }

    close(mainSock);
    
    return 0;
}