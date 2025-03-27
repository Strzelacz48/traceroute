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

uint8_t receive_reply(int sockfd, struct sockaddr_in *recv_addr, struct timeval *start_time, int ttl, int pid) {
    socklen_t addr_len = sizeof(*recv_addr); //rozmiar struktury sockaddr_in, wymagany dla funkcji recvfrom
    char recv_buf[512]; //bufor na odebrany pakiet.
    struct sockaddr_in recvd_addrss[3]; //tablica przechowująca adresy IP, z których przyszły odpowiedzi.

    int received = 0;
    double total_time = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds); // inicjalizacja fd_set, dodanie sockfd.
    struct timeval select_timeout = {TIMEOUT, 0};
    printf("%d. ", ttl);

    while (received < PACKETS_PER_STAGE) {
        //czekamy na odpowiedź
        if (select(sockfd + 1, &read_fds, NULL, NULL, &select_timeout) > 0) {
            if (recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)recv_addr, &addr_len) > 0) {
                struct timeval end;
                gettimeofday(&end, NULL);
                //dostaliśmy odpowiedź
                recvd_addrss[received] = *recv_addr;

                //rozpakowujemy headera z pakietu (może być zagnieżdżony w ttl expired albo na wierzchu)
                struct ip *ip_header = (struct ip*) recv_buf;
                ssize_t offset = 4 * (ssize_t) ip_header->ip_hl;
                struct icmphdr *header = (struct icmphdr*) (recv_buf + offset);

                if(header->type == ICMP_TIME_EXCEEDED){//nie doszliśmy do celu więc przechodzimy do kopii naszego headera 
                    struct ip *ip_header_time = (struct ip*)((u_int8_t *)header+8);
                    struct icmphdr *icmp_header_time = (struct icmphdr*)((u_int8_t *)ip_header_time + ip_header_time->ip_hl * 4);
                    if(icmp_header_time->un.echo.id == htons(pid) &&
                        icmp_header_time->un.echo.sequence / PACKETS_PER_STAGE == ttl){
                        // printf(" DEBUG1 %d %d %d %d\n",(uint16_t)pid, icmp_header_time->un.echo.id, PACKETS_PER_STAGE * ttl, icmp_header_time->un.echo.sequence);
                        #if DEBUG == 1
                            printf("%s ", inet_ntoa(recvd_addrss[received].sin_addr));
                        #endif
                        total_time += ((end.tv_sec - start_time->tv_sec) * 1000.0) + ((end.tv_usec - start_time->tv_usec) / 1000.0);
                        received++;
                    }
                } 
                else if(header->type == ICMP_ECHOREPLY &&
                        header->un.echo.id == htons(pid) 
                        && header->un.echo.sequence / PACKETS_PER_STAGE == ttl){// doszliśmy do końca
                    #if DEBUG == 1
                        // printf(" DEBUG2 %d %d %d %d\n",(uint16_t)pid, header->un.echo.id, PACKETS_PER_STAGE * ttl, header->un.echo.sequence);
                        printf("%s ", inet_ntoa(recvd_addrss[received].sin_addr));
                    #endif
                    total_time += ((end.tv_sec - start_time->tv_sec) * 1000.0) + ((end.tv_usec - start_time->tv_usec) / 1000.0);
                    received++;
                }
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
        #if DEBUG == 0
            print_unique(recvd_addrss);
        #endif
        printf("%f ms\n", (total_time / received));
    }
    else
    {
        ERROR("received too many packets\n");
    }
    return received;
}
