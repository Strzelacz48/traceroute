#include "header.h"

//324279 Marcin Czapulak

void print_unique(struct sockaddr_in recvd_addrss[3]) {
    char unique_ips[3][INET_ADDRSTRLEN];  // Tablica na unikalne IP
    int unique_count = 0;

    for (int i = 0; i < 3; i++) {
        char *current_ip = inet_ntoa(recvd_addrss[i].sin_addr);
        int is_unique = 1;

        // Sprawdź, czy adres już został dodany
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(unique_ips[j], current_ip) == 0) {
                is_unique = 0;
                break;
            }
        }

        // Jeśli jest unikalny, dodaj go do listy i wypisz
        if (is_unique) {
            strcpy(unique_ips[unique_count], current_ip);
            unique_count++;
            printf("%s ", current_ip);
        }
    }
}

void receive_reply(int sockfd, struct sockaddr_in *recv_addr, struct timeval *start_time, int ttl) {
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

                // printf("%s ", inet_ntoa(recvd_addrss[received].sin_addr));
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
        print_unique(recvd_addrss);
        printf("%f ms\n", (total_time / received));
    }
    else
    {
        ERROR("received too many packets\n");
    }    
}
