#ifndef HEADER_H
#define HEADER_H

//324279 Marcin Czapulak

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <assert.h>

#define MAX_HOPS 30
#define PACKETS_PER_STAGE 3
#define TIMEOUT 1

void print_unique(struct sockaddr_in recvd_addrss[3]);
void ERROR(const char* str);
u_int16_t compute_icmp_checksum(const void *buff, int length);
//Funkcja wysyłająca jeden pakiet 
void send_request(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid, int i);
//Funkcja do wysyłania wszystkich requestów danej fazy o tym samym ttl
void send3requests(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid);
//Funkcja odbierająca pakiety i sprawdzająca czy te pakiety są nasze
void receive_reply(int sockfd, struct sockaddr_in *recv_addr, struct timeval *start_time, int ttl);

#endif
