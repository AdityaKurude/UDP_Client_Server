#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SND_PORT    5001
#define REC_PORT    5000

#define IP      "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
 
 
void exit_error(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char *argv[]) {

    struct sockaddr_in si_me, si_other, addr;

    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char message[BUFLEN];
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        exit_error("socket");
    }

    // zero out the structure
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
        printf("Enter message : ");
        gets(message);

        if (sendto(s, message, sizeof(message), 0, (struct sockaddr *) &addr,
                sizeof(addr)) < 0) {
            perror("sendto");
            exit(1);
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            exit_error("recvfrom()");
        }
        printf("Client Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        puts(buf);
    }
    return 0;
}
