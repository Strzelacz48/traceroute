#include "header.h"

int receive_reply(int sockfd, struct sockaddr_in *recv_addr, struct timeval *start_time, int ttl, struct sockaddr_in *dest_addr) {
    socklen_t addr_len = sizeof(*recv_addr);
    char recv_buf[512];
    struct sockaddr_in recvd_addrss[3];

    int received = 0;
    double total_time = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    struct timeval select_timeout = {TIMEOUT, 0};
    printf("%d. ", ttl);

    while (received < PACKETS_PER_STAGE) {
        if (select(sockfd + 1, &read_fds, NULL, NULL, &select_timeout) > 0) {
            if (recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)recv_addr, &addr_len) > 0) {
                struct timeval end;
                gettimeofday(&end, NULL);
                // compute_icmp_checksum()
                total_time += ((end.tv_sec - start_time->tv_sec) * 1000.0) + ((end.tv_usec - start_time->tv_usec) / 1000.0);
                recvd_addrss[received] = *recv_addr;

                printf("%s ", inet_ntoa(recvd_addrss[received].sin_addr));
                received++;
            }
        } else {
            break;
        }
    }
    if (received == 0){
        printf("*\n");
    }
    else if (received < 3)
    {
        printf("???\n");
    }
    else if (received == 3)
    {
        printf("%f ms\n", (total_time / received));
    }
    else
    {
        ERROR("received too many packets\n");
    }    
    
    return received > 0 ? (int)(total_time / received) : -1;
}
