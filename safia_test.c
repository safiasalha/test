#include<stdio.h>

#include<sys/socket.h>

#include<netinet/in.h>

#include<string.h>

#include<unistd.h>



int main(){

int clientSocket, newSocket;

char message[1024];

struct sockaddr_in serverAddr;

struct sockaddr_storage serverStorage;

socklen_t addr_size;



clientSocket = socket(PF_INET,SOCK_DGRAM,0);



serverAddr.sin_family=AF_INET;



serverAddr.sin_port=htons(6426);

serverAddr.sin_addr.s_addr=inet_addr("192.168.171.133");



memset(serverAddr.sin_zero,'\0', sizeof

serverAddr.sin_zero);



bind(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));



if(listen(clientSocket,5)==0)

{

printf("Listening...\n");

}

else

printf("Error\n");



addr_size = sizeof serverStorage;

newSocket = accept(clientSocket, (struct sockaddr *) &serverStorage, 

&addr_size);



puts("Connection Establish");



recv(newSocket, message, 50, 0);

char s[] = "3333";

char a[50];

strlcpy(a,message, 15);



if(strcasecmp(a,s)==0)

{

strlcpy(message, "success", 18);

}

else



{strlcpy(message,"you will repeat this paper\n", 15);}



send(newSocket,message, 50,0);

return 0;

}


