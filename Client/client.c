#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SND_PORT    5001    //port to send data
#define REC_PORT    5000    //port on which to listen for incoming data

#define IP      "127.0.0.1"
#define BUFLEN 512  //Max length of buffer

void exit_error(char *s) {
    perror(s);
    exit(1);
}

int main(int argc, char *argv[]) {

#ifdef DEBUG
    printf("Debug mode enabled \n");
#endif

    struct sockaddr_in si_me, si_other, addr;

    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char message[BUFLEN];
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        exit_error("socket");
    }

    // set structure to zero
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(REC_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(SND_PORT);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        exit_error("bind");
    }

    while (1) {
        printf("Enter Command : ");
        fgets(message, BUFLEN, stdin);

        if (sendto(s, message, sizeof(message), 0, (struct sockaddr *) &addr,
                   sizeof(addr)) < 0) {
            exit_error("sendto");
        }

        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //receive data, a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) < 0) {
            exit_error("recvfrom()");
        }
#ifdef DEBUG
        printf("Client Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
#endif
        puts(buf);
    }
    // close the socket
    close(s);
    return 0;
}
