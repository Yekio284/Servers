#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in servAddr, clientAddr;
    struct hostent *hp;

    if (argc < 5) {
        std::cout << "INPUT: udpclient hostname port message i" << std::endl;
        return -1;
    }
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Can't get the socket.\n");
        return -2;
    }

    bzero((char *) &servAddr, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    
    bcopy(hp->h_addr, &servAddr.sin_addr, hp->h_length);

    servAddr.sin_port = htons(atoi(argv[2]));

    bzero((char *) &clientAddr, sizeof(clientAddr)) ;

    clientAddr.sin_family = AF_INET ; 
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
    clientAddr.sin_port = 0 ;

    if (bind(sock, (struct sockaddr *) &clientAddr, sizeof(clientAddr))) {
        perror("Client didn't recieve the port.\n");
        return -3;
    }

    std::cout << "CLIENT: Ready to send." << std::endl;

    for (int i = 0; i < atoi(argv[4]); i++) {
	    if (sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
       		perror("Problems with sendto.\n");
       		return -4;
    	}
	    sleep(i);
    }

    std::cout << "CLIENT: Message has been sent." << std::endl << std::endl;
    close(sock);

    return 0;
}
