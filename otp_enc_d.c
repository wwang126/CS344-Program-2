#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char* encode(char* plainText, char* keyText){
	if(strlen(keyText) < strlen(plainText)){
		perror("Key too small!");
		exit(1);
	}
	char* encodeText = malloc(sizeof(plainText));
	int i = 0;
	char in = ' ';
	while(in != '\0'){
		in = plainText[i];
		char key = keyText[i];
		printf("Plain: %c, Key: %c, ", in, key);
		//Convert chars into numbers
		if(in == ' '){
			in = 26;
		}
		else{
			in = in - 65;
		}
		//convert keys into numbers
		if(key == ' '){
			key = 26;
		}
		else{
			key = key - 65;
		}
		//Encode the character
		char encode = (in + key)%27;
		//Convert character back to char
		if(encode == 26){
			encode = ' ';
		}
		else{
			encode = encode + 65;
		}
		encodeText[i] = encode;
		printf("Encode: %c\n", encode);
		i++;
	}
	return encodeText;
}

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	//Create storage from strings
	char buffer[200000];
	char plainText[200000];
	char keyText[200000];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

	/**Start Accepting connections here**/

	while(1){
		//kill a hanging process, since you only accept one proccess at a time
		int status;
		pid_t pidWait = waitpid(-1, &status, WNOHANG);

		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		//Fork after accepting connection
		pid_t pid = fork();
		//If child
		if(pid == 0){
			// Get the message from the client and display it
			memset(buffer, '\0', 200000);
			do{
				//Read in a packet of size 64
				charsRead = recv(establishedConnectionFD, buffer + strlen(buffer), 64, 0);
				if (charsRead < 0) error("ERROR reading from socket");
				if (strstr(buffer, "^") > 0){
					break;
				}
			}while(charsRead > 0);
			printf("SERVER: I received this from the client: \"%s\"\n", buffer);
			memset(plainText, '\0', 200000);

			//Break apart plain text and key
			char curr = '0';
			int i = 0;
			while(curr != '%'){
				curr = buffer[i];
				plainText[i] = curr;
				i++;
			}
			plainText[i - 1] = '\0';
			int j = 0;
			while(curr != '^'){
				curr = buffer[i];
				keyText[j] = curr;
				i++;
				j++;
			}
			keyText[j - 1] = '\0';
			char* encodeText = encode(plainText,keyText);
			printf("Encryted text %s\n", encodeText);

			// Send message back
			memset(buffer, '\0',200000);
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			close(listenSocketFD); // Close the listening socket
			exit(0);//Kill child
		}
		//If parent
		else if(pid > 0){
			//do nothing
		}
		//If error
		else{
			error("SERVER: Fork Error!");
		}
	}
	//TODO: Remove immortality
	return 0;
}
