#ifndef HEADER_H
#define HEADER_H

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

void ERROR(const char* str);
// void print_as_bytes(unsigned char* buff, ssize_t length);
// int is_valid_ip(const char *ip);
u_int16_t compute_icmp_checksum(const void *buff, int length);
void send_request(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid);
int receive_reply(int sockfd, struct sockaddr_in *recv_addr, struct timeval *start_time, int ttl, struct sockaddr_in *dest_header);
void send3requests(int sockfd, struct sockaddr_in *dest_addr, int ttl, int pid);

#endif
