/*
	Work with character device driver
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *openfile(char *opts)
{
	FILE *fd = NULL;
	fd = fopen("/dev/cse5361", opts);
	if (!fd)
	{	printf("File error opening file\n");
	}
	return fd;
}

main(int argc, char *argv[])
{
	FILE *fd = NULL;
	char buffer[257], data[256], *remoteip, input;
	size_t count;
	int quit = 0, ch;

	
	if ( argc < 2 )
	{
		printf("Error: please indicate whether this is the [t]arget or [d]ebug machine as an argument.");
		return 0;
	}

	if ( argv[1][0] == 't' )
	{
		remoteip = "192.168.5.128";
	}
	else
	{
		remoteip = "192.168.5.130";
	}
	printf("Using destination address: %s\n", remoteip);

	// send addresses to driver
	memset(buffer, 0, 257);
	buffer[0] = 1;
	memcpy(buffer+1, remoteip, strlen(remoteip)+1);

	fd = openfile("wb");
	if (!fd) {
		exit(1);
	}
	fwrite(buffer, 1, 30, fd);
	fclose(fd);

	while(quit == 0){

		printf("\n********Main Menu********\n");
		printf("  (D) Set destination address\n");
		printf("  (W) Write to the device\n");
		printf("  (R) Read from the device\n");
		printf("  (Q) Quit\n");

		scanf("%c", &input);
		input = toupper(input);

		while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream

		switch(input)
		{
			case 'D':
				printf("Please destination IP address (ex. 192.168.67.51):\n");
				scanf("%256s", data);
				while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream
				memset(buffer, 0, 257);
				buffer[0] = 1;
				memcpy(buffer+1, data, strlen(data)+1);
				fd = openfile("wb");
				if (fd)
				{
					fwrite(buffer, 1, strlen(data)+1, fd);
					printf("Destination address set to: %s\n", data);
					fclose(fd);
				}
			break;
			case 'W':
				printf("Please input string:\n");
				scanf("%256s", data);
				while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}	//clear stream
				memset(buffer, 0, 257);
				buffer[0] = 2;
				memcpy(buffer+1, data, strlen(data)+1);
				fd = openfile("wb");
				if (fd)
				{
					fwrite(buffer, 1, strlen(data)+1, fd);
					printf("Message sent: %s\n", data);
					fclose(fd);
				}
			break;
			case 'R':
				fd = openfile("rb");
				if (fd)
				{
					count = fread(data, 1, sizeof(data)-1, fd);
					if (!count)
						printf("No data read\n");
					else
						printf("%s\n", data);
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

