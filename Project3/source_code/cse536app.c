/*
	Work with character device driver
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/kernel.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 23456 
#define MAX_PENDING 5
#define MAX_LINE 256

struct packetformat{

uint32_t record_id;

uint32_t final_clock;

uint32_t original_clock;

__be32 source_ip;

__be32 destination_ip;

uint8_t data[236];

}datf, tp, wp;


FILE *openfile(char *opts)
{
	FILE *fd = NULL;
	fd = fopen("/dev/cse5361", opts);
	if (!fd)
	{	printf("File error opening file\n");
	}
	return fd;
}

void sentom(char* data, char *monitor_ip)
{

//printf("ack send to monitor\n");
struct sockaddr_in client, server;
   struct hostent *hp;
   char buf[MAX_LINE];
   int len, ret, n;
   int s, new_s;

   bzero((char *)&server, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(0);

   s = socket(AF_INET, SOCK_DGRAM, 0);
   if (s < 0)
   {
		perror("simplex-talk: UDP_socket error");
		exit(1);
   }

   if ((bind(s, (struct sockaddr *)&server, sizeof(server))) < 0)
   {
		perror("simplex-talk: UDP_bind error");
		exit(1);
   }

   hp = gethostbyname( monitor_ip );
   if( !hp )
   {
      	fprintf(stderr, "Unknown host %s\n", "localhost");
      	exit(1);
   }

   bzero( (char *)&server, sizeof(server));
   server.sin_family = AF_INET;
   bcopy( hp->h_addr, (char *)&server.sin_addr, hp->h_length );
   server.sin_port = htons(SERVER_PORT);
   ret = sendto(s, data, 256, 0,(struct sockaddr *)&server, sizeof(server));
   if( ret <= 0)
   {
	fprintf( stderr, "Datagram Send error %d\n", ret );
   }
}
main(int argc, char *argv[])
{
	FILE *fd = NULL;
	char buffer[257], monitor_ip[30] = {"192.168.0.39"}, input, addr[30] = {"192.168.0.40"};
	size_t count;
	int quit = 0, ch;
	uint32_t tclock;
	while(quit == 0){

		printf("\n********Main Menu********\n");
		printf("  (D) Set destination address\n");
		printf("  (M) Set monitor address\n");
		printf("  (W) Write to the device\n");
		printf("  (R) Read from the device\n");
		printf("  (Q) Quit\n");
		printf("  *************************\n");
		printf("  Option: ");

		scanf("%c", &input);
		input = toupper(input);

		while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream

		switch(input)
		{
			case 'D':
				printf("Please input destination IP address (ex. 192.168.0.40):\n");
				scanf("%256s", addr);
				while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream
				memset(buffer, 0, 257);
				buffer[0] = 0;
				memcpy(buffer+1, addr, strlen(addr)+1);
				fd = openfile("wb");
				if (fd)
				{
					fwrite(buffer, 1, strlen(addr)+1, fd);
					printf("Destination address set to: %s\n", addr);
					fclose(fd);
				}
			break;
			case 'M':
				printf("Please input monitor IP address (ex. 192.168.0.39):\n");
				scanf("%30s",  monitor_ip);
				while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream
			break;
			case 'W':
				memset(&datf, 0, 256);
				printf("Please input string:\n");
				scanf("%236[^\n]", datf.data);
				while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream

				datf.record_id = 1;
				datf.final_clock = 0;
				datf.original_clock = 0;
				inet_aton("192.168.0.40", (struct in_addr *)&datf.source_ip);
				inet_aton(addr, (struct in_addr *)&datf.destination_ip);
				fd = openfile("wb");
				if (fd)
				{
					fwrite(&datf, 1, sizeof(datf), fd);
					printf("Message sent to destination PC: %s\n", datf.data);
					fclose(fd);
					memset(&wp, 0, 256);
                                        fd = openfile("rb"); 
                                        fread((char*)&wp, 1, sizeof(wp), fd);
                                        sentom((char *)&wp, monitor_ip);
					printf("Event sent to Monitor: %s\n", wp.data);
					fclose(fd);
				}

			break;
			case 'R':
				fd = openfile("rb");
				if (fd)
				{
					memset(&tp, 0, 256);
					tp.record_id =5;
					count = fread(&tp, 1, sizeof(tp)-1, fd);
					if (!count)
						printf("No data read\n");
					else if(tp.record_id == 0 /*&& tp.source_ip == datf.source_ip && tp.destination_ip == datf.destination_ip*/)
					{
						sentom((char*)&tp, monitor_ip);
						printf("Ack sent to Monitor: %s\n", tp.data);
					}else if(tp.record_id == 1)
					{
						printf("%s\n", tp.data);
					}
					fclose(fd);
				}
			break;
			case 'Q':
				quit = 1;
			break;
			default :
				printf("Unknown Entry\n");
				break;
		}
	}

}

