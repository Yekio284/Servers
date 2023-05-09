#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating server socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = 0;

    if (bind(server_socket, (sockaddr*) &server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding server socket" << std::endl;
        return 1;
    }

    if (listen(server_socket, 10) == -1) {
        std::cerr << "Error listening on server socket" << std::endl;
        return 1;
    }

    int length = sizeof(struct sockaddr);

    getsockname(server_socket, (struct sockaddr *) &server_address, (socklen_t *) &length);
    std::cout << "PORT: " << ntohs(server_address.sin_port) << std::endl;

    while (true) {
        sockaddr_in client_address{};
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*) &client_address, &client_address_size);
        if (client_socket == -1) {
            std::cerr << "Error accepting client socket" << std::endl;
            continue;
        }

        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            std::cerr << "Error receiving data from client socket" << std::endl;
            close(client_socket);
            continue;
        }

        std::string request(buffer, bytes_received);
        std::string request_line = request.substr(0, request.find("\r\n"));
        std::cout << "Request: " << request_line << std::endl;

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html lang=\"ru\"><title>HTTP-Server</title><head><meta charset=\"UTF-8\"></head><body><h1>HTTP/1.1 200 OK</h1><h2>Host: localhost</h2><p>Content-Type: text/html</p><button onclick=\"clickme()\">Нажми на меня!</button><script type=\"text/javascript\">function clickme() {\n\tvar colors = [\'blue\', \'grey\', \'black\', \'white\', \'red\', \'green\', \'#aaa\', \'#FFAACC\', \'rgb(122,111,110)\'];\n\tdocument.body.style.backgroundColor = colors[Math.floor(Math.random() * colors.length)];\n\talert(\"Цвет страницы будет изменён! (или, если повезёт, на тот же, что и сейчас)\");\n}</script></body></html>";

        int bytes_sent = send(client_socket, response.c_str(), response.length(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Error sending data to client socket" << std::endl;
            close(client_socket);
            continue;
        }

        close(client_socket);
    }
    close(server_socket);

    return 0;
}
