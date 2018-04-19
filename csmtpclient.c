#include <stdlib.h>

#include <string.h>

#include <errno.h>

#include <netinet/in.h>//Internet address family

#include <netdb.h>//definitions for network database operations

#include <arpa/inet.h>//definitions for internet operations

#include <sys/types.h>

#include <sys/socket.h>

#include  <fcntl.h>

#include <unistd.h>

#include <stdio.h>

#include <netdb.h>

//----------variables----------------------------------------------------------------------------//

int con;//socket connect variable

int socket_fd;//create socket variable

int portno;//the port number

struct sockaddr_in gmail_server;//this struct contains ip address and a 
port of the server.

struct hostent* gmail_info;//this struct contains all the info of a host 
name in the Internet.

#define TMP_LENGTH 1024//initial size of tmp buffers

char myHostName[TMP_LENGTH];//string to store my host name

char* msg;//string to store msg body

char s[3];//string to store 's: '

char c[3];//string to store 'c: '

char* suffix;//string to store server suffix

char* buffer;//dynamically allocated char array to get messages from the 
server

#define INITIAL_SIZE 10//initial size of 'buffer'

int length;//current size of buffer

//--------------------------------------------------------------------------------------//





//---------methods-----------------------------------------------------------------------------//

void copyFileToString(char* file);

void checkServerReturnedCode(char* buf);

void open_TCP_Socket();

void get_Server_Info(char* SMTP_Server_Host_Name);

void getSuffix(char* buf);

void send_To_Server();

void sendData(char* data,int toRead);

int getLine(int fd, char* line, int lim);

//-----------MAIN---------------------------------------------------------------------------//

int main(int argc, char **argv)

{

	if(argc<3){

		printf("please enter at least 3 arguments\n");

		exit(0);

	}

	if(argc==3){

		portno=25;

	}

	else if(argc==4){

		portno=atoi(argv[3]);

	}

	//------initializing 
variables--------------------------------------------------------------------------------//

	strcpy(s,"s: ");

	strcpy(c,"c: ");

	copyFileToString(argv[1]);

	
//--------------------------------------------------------------------------------------//

//-----connecting to the server and sending the 
e-mail---------------------------------------------------------------------------------//

	get_Server_Info(argv[2]);

	open_TCP_Socket();

	send_To_Server();

	//------freeing dynamically allocated 
memory--------------------------------------------------------------------------------//

	free(suffix);

	free(msg);

	
//-------------------------------------------------------------------------------------//



	return 0;

}/this method copies all the input file data to "char* msg" dynamically.

void copyFileToString(char* file){

	//------opening 
file--------------------------------------------------------------------------------//

	FILE* fp;

	fp = fopen(file,"r");

	if( fp == NULL )

	{

		printf("%s\n", strerror(errno));

		exit(0);

	}

	
//--------------------------------------------------------------------------------------//



	//-----initializing 
variables---------------------------------------------------------------------------------//

	msg=(char*)malloc(INITIAL_SIZE);

	char tmp[INITIAL_SIZE];

	int msg_length=INITIAL_SIZE;

	int tmp_length=INITIAL_SIZE;

	bzero(msg,msg_length);

	bzero(tmp,tmp_length);

	int bytes=0;

	int n=0;

	msg[0]='\0';

	while(1){		bzero(tmp,tmp_length);

		n=fread(tmp, 1, tmp_length-1, fp);

		if(n<0){

			printf("%s\n", strerror(errno));

			exit(0);

		}

		if(n==0)

			break;



		bytes+=n;

		//---checking if the buffer has enough length to contain 
the part of the 
message---------------------------------------------------------------------------------//

		if(bytes<msg_length){

			strcat(msg,tmp);

		}

		//----checking if the buffer does not has enough length 
to contain the part of the message

		//therefore it need to be 
realloced----------------------------------------------------------------------------------//

		if(bytes>=msg_length){

			msg_length=bytes;

			msg=(char*)realloc(msg,msg_length+1);

			strcat(msg,tmp);

		}

	}	fclose(fp);



}

void checkServerReturnedCode(char* buf){

	//-----getting the code of the 
msg---------------------------------------------------------------------------------//

	char server_returned_code[4]="   ";

	memcpy(server_returned_code, buf,strlen(server_returned_code));

	int code=0;

	code=atoi(server_returned_code);

	//------checking if the code is 
valid--------------------------------------------------------------------------------//

	if(code<200 ||code>300){

		printf("code number:%d\n",code);

		printf("ERROR:%s\n",buf);

		exit(0);

	}



}//this method gets all the info about the server.

void get_Server_Info(char* SMTP_Server_Host_Name){

	//----getting all the information about 
gmail----------------------------------------------------------------------------------//

	char* gmail_ip;

	//gethostbyname() returns strcut hostent contains all the info 
about the host name argument

	gmail_info=gethostbyname(SMTP_Server_Host_Name);

	if(gmail_info==NULL){

		printf("%s\n", strerror(errno));

		exit(0);

	}

	
//--------------------------------------------------------------------------------------//





	//----printing the ip and the name of the 
server----------------------------------------------------------------------------------//

	gmail_ip=(char*)malloc(INET_ADDRSTRLEN+1);

	inet_ntop(AF_INET, gmail_info->h_addr, gmail_ip, 
INET_ADDRSTRLEN);

	printf("server IP:%s\n",gmail_ip);

	free(gmail_ip);

	printf("server 
Name:%s\n",(char*)gmail_info->h_name);//--------filling struct 
sockaddr_in gmail_server in the ip and the port of the server (from 
struct hostent* 
gmail_info)------------------------------------------------------------------------------//

	bzero(&gmail_server,sizeof(gmail_server));

	gmail_server.sin_family=AF_INET;//AF_INIT means Internet doamin 
socket.

	gmail_server.sin_port=htons(25);//port 25=SMTP.

	bcopy((char *)gmail_info->h_addr,(char 
*)&gmail_server.sin_addr.s_addr,gmail_info->h_length);

}/this method opens a TCP socket with the server.

void open_TCP_Socket(){

	char buf[TMP_LENGTH];

	//------opening TCP  
socket--------------------------------------------------------------------------------//

	socket_fd= socket(AF_INET,SOCK_STREAM,0);

	if(socket_fd<0){

		perror("ERROR");

		exit(0);

	}

	
//--------------------------------------------------------------------------------------//





	//----connecting to to the server via above 
socket----------------------------------------------------------------------------------//

	con=connect(socket_fd,(struct sockaddr 
*)&gmail_server,sizeof(gmail_server));

	if(con<0){

		perror("ERROR");

		exit(0);

	}//----getting connection message from the 
server----------------------------------------------------------------------------------//

	getLine(socket_fd,buf,TMP_LENGTH);

	checkServerReturnedCode(buf);

	printf("%s",s);

	printf("%s\n",buf);

	getSuffix(buf);

	
//--------------------------------------------------------------------------------------//



}
