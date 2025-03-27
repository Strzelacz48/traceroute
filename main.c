#include "header.h"
//324279 Marcin Czapulak
int main(int argc, char *argv[]) {
    //sprawdzanie ilości argumentów
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP addres >\n", argv[0]); //Nie uzywam ERROR bo argv[0], łatwiej zrobić wyjątek
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));//upewnienie się że nie będzie losowych danych
    dest_addr.sin_family = AF_INET;
    //sprawdzanie poprawności danych
    if (inet_pton(AF_INET, argv[1], &dest_addr.sin_addr) != 1) ERROR("Incorrect IP addres");
    //Przygotowanie socketa
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) ERROR("socket failed");
    
    struct timeval timeout = {TIMEOUT, 0};//sekunda na czekanie
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    int pid = getpid();
    //główna pętla
    struct sockaddr_in recv_addr;
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++) {
        struct timeval start_time;
        gettimeofday(&start_time, NULL);
        
        send3requests(sockfd, &dest_addr, ttl, pid);
        uint8_t reached_and_printed = receive_reply(sockfd, &recv_addr, &start_time, ttl, pid);
        //jeśli doszliśmy do celu wyjdź z pętli
        if (recv_addr.sin_addr.s_addr == dest_addr.sin_addr.s_addr && reached_and_printed > 0) {
            //printf("Destination reached\n");
            break;
        }
    }
    
    close(sockfd);
    return EXIT_SUCCESS;
}