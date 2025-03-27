#include "header.h"

//324279 Marcin Czapulak
void ERROR(const char* str) {
    fprintf(stderr, "%s: %s\n", str, strerror(errno));
    exit(EXIT_FAILURE);
}
// Z wykładu
u_int16_t compute_icmp_checksum(const void *buff, int length) {
    const u_int16_t* ptr = (const u_int16_t*)buff;
    u_int32_t sum = 0;
    assert(length % 2 == 0);
    for (; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16U) + (sum & 0xffffU);
    return ~(sum + (sum >> 16U));
}

void send_request(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid, int i) {
    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.un.echo.id = htons(pid);
    icmp_hdr.un.echo.sequence = PACKETS_PER_STAGE * ttl + i;
	icmp_hdr.checksum = 0;// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ALE TO JEST GŁUPIE GODZINA DEBUGA PRZEZ TO ŻE CHECKSUM DODAWAĆ NIE UMIE BEZ ZERA
    icmp_hdr.checksum = compute_icmp_checksum((uint16_t*)&icmp_hdr, sizeof(icmp_hdr));
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    #if DEBUG == 1   
        printf("id : %d htons(pid): %d sequence: %d math : %d \n",icmp_hdr.un.echo.id, htons(pid), icmp_hdr.un.echo.sequence, PACKETS_PER_STAGE * (ttl - 1) + i);
    #endif
    if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr)) < 0){
        ERROR("sendto error ");
    };
}
//Funkcja odbierająca pakiety i sprawdzająca czy te pakiety są nasze
void send3requests(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid){
    //Nie jestem pewien czy to wystarczyło ale razem z innymi zmianami to naprawiło brak zwrotów więc zostawiam w dwóch funkcjach
    for (int i = 0; i < PACKETS_PER_STAGE; i++) 
        send_request(sockfd, dest_addr, ttl, pid, i);
}
