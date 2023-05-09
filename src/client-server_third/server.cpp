#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/syscall.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* client_handler(void *arg) {
    std::ofstream out;
    out.open("output.txt", std::ios::app);

    int accSock = *((int *)arg);
    char buffer[sizeof(int)];

    if (pthread_mutex_unlock(&mutex) != 0) {
        std::cerr << "Error with pthread_mutex_unlock() in client handler" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (recv(accSock, buffer, sizeof(int), 0) > 0 && out.is_open()) {
        if (pthread_mutex_lock(&mutex) != 0) {
            std::cerr << "Error with pthread_mutex_lock() in client handler" << std::endl;
        }

        std::cout << "Recieved new message from id: " << syscall(SYS_gettid) << "\tMESSAGE: " << buffer << std::endl;
        out << "id: " << syscall(SYS_gettid) << "\tMESSAGE: " << buffer << std::endl;

        if (pthread_mutex_unlock(&mutex) != 0) {
            std::cerr << "Error with pthread_mutex_unlock() in client handler" << std::endl;
        }
    }
    
    close(accSock);
    out.close();
   
    return 0;
}

int main() {
    int mainSock, accSock, length = sizeof(struct sockaddr);
    struct sockaddr_in servAddr, clAddr;
    pthread_t thread;

    if ((mainSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error with socket()" << std::endl;
        exit(EXIT_FAILURE);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = 0;

    if (bind(mainSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        std::cerr << "Error with bind()" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (getsockname(mainSock, (struct sockaddr *) &servAddr, (socklen_t *) &length) == -1) {
        std::cerr << "Error with getsockname()" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "PORT: " << ntohs(servAddr.sin_port) << std::endl;

    if (listen(mainSock, 3) == -1) {
        std::cerr << "Error with listen()" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        if ((accSock = accept(mainSock, (struct sockaddr *) &clAddr, (socklen_t *) &length)) == -1) {
            std::cerr << "Error with accept()" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pthread_mutex_lock(&mutex) != 0) {
            std::cerr << "Error with pthread_mutex_lock() in main" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&thread, NULL, client_handler, &accSock) != 0) {
            std::cerr << "Error with pthread_create()" << std::endl;
            close(accSock);
            continue;
        }

        if (pthread_detach(thread) != 0) {
            std::cerr << "Error with pthread_detach()" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    close(mainSock);

    return 0;
}