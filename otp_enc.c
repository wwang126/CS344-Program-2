#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//Parse a text file and return a array of chracter, 0 if failed
char* readFile(char* fileName){
	//Read in plain text file
	FILE *f = fopen(fileName, "r");
	//Move file pointer to end of file
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	//rewind point to front
	fseek(f, 0, SEEK_SET);

	char *textOut = malloc(fsize + 1);
	fread(textOut, fsize, 1, f);
	fclose(f);
	//add null terminator
	textOut[strcspn(textOut, "\n")] = 0;

	//File verification
	int i = 0;
	while(i < fsize){
		char curr = textOut[i];
		if(curr != 32){2
			if(curr < 65 || curr > 90){
				printf("Error!");
				free(textOut);
				exit(1);
			}
		}
		i++;
	}

	return textOut;
}

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[65536];//64kilo byte file

	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	//read in plain text file
	char* plainText = readFile(argv[1]);

	//read in key text file
	char* keyText = readFile(argv[2]);

	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer));
	//Copy text file into buffer
	strcpy(buffer,plainText);

	// Send message to server
	// Write to the server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}
