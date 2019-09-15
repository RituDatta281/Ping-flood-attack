#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//#include<libexplain/socket.h>
#include "myheader.h"

/*************************************************************
  Given an IP packet, send it out using a raw socket.
**************************************************************/
void send_raw_ip_packet(struct ipheader* ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Step 1: Create a raw network socket.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sock < 0){
        printf("Sock open problem\n");
       // printf("%s\n",explain_socket(AF_INET, SOCK_RAW, IPPROTO_RAW));
    }

    // Step 2: Set socket option.
    int setopt = setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
                     &enable, sizeof(enable));
    if(setopt < 0){
        printf("Set ERROR\n");
    }
    // Step 3: Provide needed information about destination.
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr = ip->iph_destip;

    // Step 4: Send the packet out.
    //printf("The ip header length -> %d\n",ntohs(ip->iph_len));
    int sendopt =  sendto(sock, ip, ntohs(ip->iph_len), 0,
           (struct sockaddr *)&dest_info, sizeof(dest_info));

    if(sendopt < 0){
        printf("Send ERROR\n");
    }
    close(sock);
}

