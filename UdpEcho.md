# Introduction #

In case of need, a simple UDP echo program example.

# Details #
```
#include <netinet/in.h>
#include <sys/socket.h>

void
udp_echo(int udp_port)
{
    struct sockaddr_in si_bind, si_rx, si_tx;
    int sock_rx, sock_tx, slen=sizeof(si_tx);
    int i;
    int len_rx, len_tx;
    char buf[256];
    
    // open two sockets for each direction
    sock_rx=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sock_tx=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (sock_rx == -1)
    {
        fprintf(stderr, "ERROR: sock_rx\n");
        exit(-1);
    }
    if (sock_tx == -1)
    {
        fprintf(stderr, "ERROR: sock_tx\n");
        exit(-1);
    }
    
    memset((char *) &si_tx, 0, sizeof(si_tx));
    si_tx.sin_family = AF_INET;
    si_tx.sin_port = htons(udp_port);
    if (inet_aton("127.0.0.1", &si_tx.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(-1);
    }

    memset((char *) &si_bind, 0, sizeof(si_bind));
    si_bind.sin_family = AF_INET;
    si_bind.sin_port = htons(enable_forward);
    si_bind.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock_rx, (struct sockaddr *)&si_bind, sizeof(si_bind)) == -1) {
        fprintf(stderr, "bind() failed\n");
        exit(-1);
    }


    for (i=0; i < 10; i++) {
        printf("Receiving packet %d\n", i);
        len_rx = recvfrom(sock_rx, buf, sizeof(buf), 0, (struct sockaddr *)&si_rx, &slen);
        if (len_rx == -1) {
            fprintf(stderr, "ERROR: recvfrom\n");
            exit(-1);
        }

        printf("Received packet from %s:%d\nData(%d): %s\n\n", 
             inet_ntoa(si_rx.sin_addr), ntohs(si_rx.sin_port), len_rx, buf);
        
        if (sendto(sock_tx, buf, len_rx, 0, (struct sockaddr *)&si_tx, slen) == -1)
        {
            fprintf(stderr, "ERROR: sendto\n");
            exit(-1);
        }
        
    }
    
    close(sock_rx);
    close(sock_tx);
}
```