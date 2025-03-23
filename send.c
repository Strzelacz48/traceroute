#include "header.h"

void ERROR(const char* str) {
    fprintf(stderr, "%s: %s\n", str, strerror(errno));
    exit(EXIT_FAILURE);
}

// void print_as_bytes(unsigned char* buff, ssize_t length) {
//     for (ssize_t i = 0; i < length; i++, buff++)
//         printf("%.2x ", *buff);
// }

// int is_valid_ip(const char *ip) {
//     struct sockaddr_in sa;
//     return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
// }

u_int16_t compute_icmp_checksum(const void *buff, int length) {
    const u_int16_t* ptr = (const u_int16_t*)buff;
    u_int32_t sum = 0;
    assert(length % 2 == 0);
    for (; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16U) + (sum & 0xffffU);
    return ~(sum + (sum >> 16U));
}

void send_request(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid) {
    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.un.echo.id = pid;
    icmp_hdr.un.echo.sequence = PACKETS_PER_STAGE * (ttl - 1);
	icmp_hdr.checksum = 0;// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ALE TO JEST GŁUPIE GODZINA DEBUGA PRZEZ TO ŻE CHECKSUM DODAWAĆ NIE UMIE BEZ ZERA
    icmp_hdr.checksum = compute_icmp_checksum((uint16_t*)&icmp_hdr, sizeof(icmp_hdr));
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr)) < 0){
        ERROR("sendto error ");
    };
}

void send3requests(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid){
    for (int i = 0; i < PACKETS_PER_STAGE; i++) 
        send_request(sockfd, dest_addr, ttl, pid);
}
