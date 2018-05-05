

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SND_PORT    5000
#define REC_PORT    5001

#define IP      "127.0.0.1"
#define BUFLEN 512  //Max length of buffer

  
void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in addr, srcaddr;
    int fd, rec_fd;

    char message[BUFLEN];
    char buf[BUFLEN];

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(SND_PORT);

    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.sin_family = AF_INET;
    srcaddr.sin_addr.s_addr = inet_addr(IP);
    srcaddr.sin_port = htons(REC_PORT);

    if (bind(fd, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
        perror("bind");
        exit(1);
    }


    while (1) {
        printf("Enter message : ");
        gets(message);

        if (sendto(fd, message, sizeof(message), 0, (struct sockaddr *) &addr,
                sizeof(addr)) < 0) {
            perror("sendto");
            exit(1);
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *) &addr, sizeof(addr)) == -1)
        {
            die("recvfrom()");
        }

        puts(buf);
    }
    return 0;
}
