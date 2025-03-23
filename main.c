#include "header.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <adres IP>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &dest_addr.sin_addr) != 1) {
        fprintf(stderr, "Nieprawidłowy adres IP\n");
        return EXIT_FAILURE;
    }
    
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) ERROR("socket failed");
    
    struct timeval timeout = {TIMEOUT, 0};//sekunda na czekanie
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    struct sockaddr_in recv_addr;
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++) {
        struct timeval start_time;
        gettimeofday(&start_time, NULL);
        
        struct icmphdr dest_header = send_request(sockfd, &dest_addr, ttl);
        receive_reply(sockfd, &recv_addr, &start_time, ttl, &dest_header);
        
        if (recv_addr.sin_addr.s_addr == dest_addr.sin_addr.s_addr) {
            break;
        }
    }
    
    close(sockfd);
    return EXIT_SUCCESS;
}