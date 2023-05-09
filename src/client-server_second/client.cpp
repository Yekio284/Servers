#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
 
int main(int argc, char *argv[]) {
    int mainSock;
    struct sockaddr_in servAddr;
     
    if (argc != 4) {
        std::cout << "Incorrect input. Example: ./client <IP> <PORT> <message>" << std::endl;
        exit(1);
    }

    mainSock = socket(AF_INET, SOCK_STREAM, 0);
    if (mainSock == -1) {
        std::cerr << "Error with socket() " << std::endl;
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));
    
    if (connect(mainSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        std::cerr << "Error with connect() " << std::endl;
        exit(2);
    }
    
    for (int i = 0; i < atoi(argv[3]); i++) {
	    if (send(mainSock, argv[3], strlen(argv[3]), 0) == -1) {
       		std::cerr << "Error with send() " << std::endl;
       		exit(3);
    	}
        std::cout << "Send to server: " << atoi(argv[3]) << std::endl;
        
        char answerFromServ[64];
        if (recv(mainSock, answerFromServ, 64, 0) == -1) {
            std::cerr << "Error with recv() " << std::endl;
            exit(4);
        }
        std::cout << "Recieved from server: " << answerFromServ << std::endl;

        sleep(atoi(argv[3]));
    }

    close(mainSock);
    return 0;
}