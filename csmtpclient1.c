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

int con;//socket connect variable

int socket_fd;//create socket variable

int portno;//the port number


struct sockaddr_in gmail_server;//this struct contains ip address and a port of the server.

struct hostent* gmail_info;//this struct contains all the info of a host name in the Internet.

#define TMP_LENGTH 1024

char myHostName[TMP_LENGTH];//string to store my host name

char* msg;//string to store msg body

char s[3];//string to store 's: '

char c[3];//string to store 'c: '

char* suffix;//string to store server suffix

char* buffer;//dynamically allocated char array to get messages from the server

#define INITIAL_SIZE 10


int* length;


void copyFileToString(char* file);

void checkServerReturnedCode(char* buf);

void open_TCP_Socket();

void get_Server_Info(char* SMTP_Server_Host_Name);

void getSuffix(char* buf);

void send_To_Server();

void sendData(char* data,int toRead);

int getLine(int fd, char* line, int lim);

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

	strcpy(s,"s: ");

	strcpy(c,"c: ");

	copyFileToString(argv[1]);

	get_Server_Info(argv[2]);

	open_TCP_Socket();

	send_To_Server();

	free(suffix);

	free(msg);

return 0;

}


void copyFileToString(char* file)
{
	FILE* fp;

	fp = fopen(file,"r");

	if( fp == NULL )

	{

		printf("%s\n", strerror(errno));

		exit(0);

	}

	msg=(char*)malloc(INITIAL_SIZE);

	char tmp[INITIAL_SIZE];

	int msg_length=INITIAL_SIZE;

	int tmp_length=INITIAL_SIZE;

	bzero(msg,msg_length);

	bzero(tmp,tmp_length);

	int bytes=0;

	int n=0;

	msg[0]='\0';

	while(1){

		bzero(tmp,tmp_length);

		n=fread(tmp, 1, tmp_length-1, fp);

		if(n<0){

			printf("%s\n", strerror(errno));

			exit(0);

		}

		if(n==0)

			break;



		bytes+=n;

if(bytes<msg_length){

			strcat(msg,tmp);

		}

		if(bytes>=msg_length){

			msg_length=bytes;

			msg=(char*)realloc(msg,msg_length+1);

			strcat(msg,tmp);

		}

	}

	fclose(fp);



}

void checkServerReturnedCode(char* buf){

char server_returned_code[4]="   ";

	memcpy(server_returned_code, buf,strlen(server_returned_code));

	int code=0;

	code=atoi(server_returned_code);

	if(code<200 ||code>300){

		printf("code number:%d\n",code);

		printf("ERROR:%s\n",buf);

		exit(0);

	}



}

void get_Server_Info(char* SMTP_Server_Host_Name){

	char* gmail_ip;
	gmail_info=gethostbyname(SMTP_Server_Host_Name);

	if(gmail_info==NULL){

		printf("%s\n", strerror(errno));

		exit(0);

	}

	gmail_ip=(char*)malloc(INET_ADDRSTRLEN+1);

	inet_ntop(AF_INET, gmail_info->h_addr, gmail_ip, INET_ADDRSTRLEN);

	printf("server IP:%s\n",gmail_ip);

	free(gmail_ip);

	printf("server Name:%s\n",(char*)gmail_info->h_name);

	bzero(&gmail_server,sizeof(gmail_server));

	gmail_server.sin_family=AF_INET;//AF_INIT means Internet doamin socket.

	gmail_server.sin_port=htons(25);//port 25=SMTP.

	bcopy((char *)gmail_info->h_addr,(char *)&gmail_server.sin_addr.s_addr,gmail_info->h_length);

}

void open_TCP_Socket(){

	char buf[TMP_LENGTH];
	socket_fd= socket(AF_INET,SOCK_STREAM,0);

	if(socket_fd<0){

		perror("ERROR");

		exit(0);

	}

	con=connect(socket_fd,(struct sockaddr *)&gmail_server,sizeof(gmail_server));

	if(con<0){

		perror("ERROR");

		exit(0);

	}

	getLine(socket_fd,buf,TMP_LENGTH);

	checkServerReturnedCode(buf);

	printf("%s",s);

	printf("%s\n",buf);

	getSuffix(buf);
}

void getSuffix(char* buf){

	char *token;

	char* tmp;

	const char space[3]=" ";

	token = strtok(buf,space);

	/* walking through other tokens until we reached the last word 
which is the suffix */

	while(1)

	{

		tmp=malloc(strlen(token)+1);

		strcpy(tmp,token);

		token = strtok(NULL,space);

		if(token == NULL)

			break;

		free(tmp);

	}

	suffix=malloc((sizeof(char)*strlen(tmp))+1);

	strcpy(suffix,tmp);

	free(tmp);

}

void send_To_Server(){

	char buf[TMP_LENGTH];

	char *token;

	const char line[3]="\n";



	bzero(buf,TMP_LENGTH);

	gethostname(myHostName,TMP_LENGTH);

	strcpy(buf,"HELO ");

	strcat(buf,myHostName);

	strcat(buf,"\n");

	sendData(buf,0);

	bzero(buf,TMP_LENGTH);

	getLine(socket_fd,buf,TMP_LENGTH);

	checkServerReturnedCode(buf);

	printf("%s",s);

	printf("%s\n",buf);

	bzero(buf,TMP_LENGTH);

	sprintf(buf,"MAIL FROM:");

	token = strtok(msg,line);

	strcat(buf,token);

	strcat(buf,"\n");

	sendData(buf,1);

	bzero(buf,TMP_LENGTH);

	strcpy(buf,"RCPT TO:");

	token = strtok(NULL,line);

	strcat(buf,token);

	strcat(buf,"\n");

	sendData(buf,1);

	bzero(buf,TMP_LENGTH);

	strcpy(buf,"DATA\n");

	sendData(buf,1);

	token = strtok(NULL,line);


while(strlen(token)!=1){

		bzero(buf,TMP_LENGTH);

		strcpy(buf,token);

		strcat(buf,"\n");

		sendData(buf,0);

		token = strtok(NULL,line);

	}

	while(token!=NULL){

		sendData(token,0);

		printf("\n");

		token = strtok(NULL,line);

	}

	sendData("\r\n.\r\n",1);

	bzero(buf,TMP_LENGTH);

	strcpy(buf,"QUIT\n");

	sendData(buf,1);
}

void sendData(char* data,int toRead){
	int n=0;

	n=write(socket_fd, data, strlen(data));

	if(n<0){

		printf("%s\n", strerror(errno));

		exit(0);

	}

	if(strcmp(data,"\r\n.\r\n")==0){

		printf("%s",c);

		printf("%s\n\n",".");

	}

	else{

	printf("%s",c);

	printf("%s\n",data);

	}

	if(toRead==1){
		char tmp[INITIAL_SIZE];//tmp string to store parts of the message

		int tmp_length=INITIAL_SIZE;

		int bytes=0;
		buffer=(char*)malloc(INITIAL_SIZE);

		int length=INITIAL_SIZE;

		bzero(buffer,length);

		bzero(tmp,tmp_length);

		while(1){

	bzero(tmp,tmp_length);

		bytes+=read(socket_fd,tmp,tmp_length-1);//it reads length-1 to save space for '\0'.

		if(bytes<0){

		printf("%s\n", strerror(errno));

		exit(0);

	}

	if(bytes<length){

		strcat(buffer,tmp);

	}

		if(bytes>=length){

				length=bytes;

				buffer=(char*)realloc(buffer,length+1);

				strcat(buffer,tmp);

			}

	if(strstr(buffer,suffix)!=NULL){

				break;

			}

		}

	if(strcmp(data,"DATA\n")!=0)

			checkServerReturnedCode(buffer);

	printf("%s",s);

		printf("%s\n",buffer);

		free(buffer);

	}

}

int getLine(int fd, char* line, int lim)

{

	int i;

	char c;



	i =  0;

	while (--lim > 0 && read(fd,&c,1)>0 && c!='\n' && c!='\0')

	{

		line[i++] = c;

	}

	if (c=='\n')

		line[i++] = c;

	line[i] = '\0';

	return i;

}
