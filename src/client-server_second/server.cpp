#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
 
void zombie_killer(int sig) {
    pid_t num = waitpid(-1, NULL, WNOHANG);
    while(waitpid(-1, NULL, WNOHANG) > 0);
    std::cout << "Child process " << num << " killed..." << std::endl << std::endl;
}
 
int main() {
    int accSock, mainSock, msg_size, length = sizeof(struct sockaddr);
    struct sockaddr_in servAddr;
    char buf[1024];

    mainSock = socket(AF_INET, SOCK_STREAM, 0);
    if(mainSock < 0) {
        std::cerr << "Error with socket() " << std::endl;
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;
    
    if(bind(mainSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        std::cerr << "Error with bind() " << std::endl;
        exit(2);
    }

    listen(mainSock, 3);
    getsockname(mainSock, (struct sockaddr *) &servAddr, (socklen_t *) &length);
    std::cout << "PORT: " << ntohs(servAddr.sin_port) << std::endl;
    signal(SIGCHLD, zombie_killer);
    
    int k = 0;
    while (true) {
        accSock = accept(mainSock, 0, 0);
        if(accSock < 0) {
            std::cerr << "Error with accept() " << std::endl;
            exit(3);
        }   
        
        pid_t t = fork();
        if (t == -1) {
            std::cerr << "Error with fork() " << std::endl;
            break;
        }
        else if (t == 0) {
            close(mainSock);
            
            while (true) {
                msg_size = recv(accSock, buf, 8, 0);
                if (msg_size == 0) {
                    break;
                }
                else if (msg_size == -1) {
                    std::cerr << "Error with recv() " << std::endl;
                    exit(4);
                }
                
                std::cout << "Recieved new message: " << buf[0] << std::endl;
        
                char answer[64] = "This is server. I've got your message!";
                int s = send(accSock, answer, sizeof(answer), 0);
                if (s == -1) {
                    std::cerr << "Error with send() " << std::endl;
                    exit(5);
                }
                std::cout << "Send message to client." << std::endl << std::endl;
            }
            
            close(accSock);
            exit(0);
        }
        else {
            close(accSock);
        }
    } 

    close(mainSock);
    return 0;
}