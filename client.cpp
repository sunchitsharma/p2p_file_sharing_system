#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define ERROR -1
#define BUFFER 1024
using namespace std;


struct sockaddr_in c_addr;
char fname[100]={0};
char rfname[100]={0};

int sendfile(int *x)
{
      int clifd=(int)*x;
      printf("Connection accepted and id: %d\n",clifd);
      printf("Connected to Clent: %s:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
      write(clifd, fname,256);

        FILE *fp = fopen(rfname,"rb");
        if(fp==NULL)
        {
            printf("File open error : %s",rfname);
            return 1;   
        }   
        while(1)
        {
            unsigned char buff[1024]={0};
			int dat= fread(buff,1,1024,fp);
            if((dat)> 0)
            {
                write(clifd, buff, dat);
            }
            if (dat < 1024)
            {
                if (feof(fp))
		{
                    printf("End of file\n");
		    printf("File transfer completed for id: %d\n",clifd);
		    break;
		}
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }
printf("Closing Connection for id: %d\n",clifd);
close(clifd);
shutdown(clifd,SHUT_WR);
return 0;
}

int serverprog(int sport)
{
    int clifd = 0,err;
    pthread_t tid; 
    struct sockaddr_in serv_addr;
    int sock = 0;
    char obuff[1025];
    int numrv;
   socklen_t  clen=0;

   if((sock = socket(AF_INET, SOCK_STREAM,0))==-1)
	{
		perror("Socket Error:/n");
		exit(-1);
	}

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    serv_addr.sin_port = htons(sport);
	int len =sizeof(serv_addr);

    if((bind(sock, (struct sockaddr *)&serv_addr, len))==-1)
	{
		perror("bind: /n");
		exit(-1);
	}

    if(listen(sock, 7) == -1)
    {
        perror("Failed to listen\n");
        return -1;
    }

    while(1)
    {
        clen=sizeof(c_addr);
        printf("Waiting......%u %d\n",serv_addr.sin_addr.s_addr,sport);
        clifd = accept(sock, (struct sockaddr*)&c_addr,&clen);
        //char local[100]={0};
        recv(clifd, rfname, 100,0);
        printf("REC at server prog : %s",rfname);
        if(clifd<0)
        {
	  printf("Error in accept\n");
	  continue;	
	}
        sendfile(&clifd);
   }
    close(clifd);
    return 0;
}



void gotoxy(int x,int y)
 {
 printf("%c[%d;%df",0x1B,y,x);
 }
int downloadfunc(char* ip,char* prt,char* filetry)
{
    system("clear");
    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;
    
    //create a socket for new download
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    //define socket attr.
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(prt));
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    
    //printing rec ip and port
    printf("IP RECV : %s  PORT RECV: %d",ip,atoi(prt));
    
    //connecting to the rec server
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    printf("Connected to ip: %s : %d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
    FILE *fp;
	send(sockfd, filetry, sizeof(filetry),0);
	printf("File Name: %s\n",filetry);
	printf("Receiving file...");
   	 fp = fopen(filetry, "ab"); 
    	if(NULL == fp)
    	{
       	 printf("Error opening file");
         return 1;
    	}
    long double sz=1;
    while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0)
    { 
        sz++;
        gotoxy(0,4);
        printf("Received: %llf Mb",(sz/1024));
	fflush(stdout);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        // printf("%s \n", recvBuff);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
    printf("\nFile OK....Completed\n");
    return 0;
}


int main(int argc, char **argv)
{
	int sport;
	printf("Enter your server port :");
	cin>>sport;
	int f=fork();
	if(f) serverprog(sport);
	else
	{
	struct sockaddr_in remote_server;
	int sock;
	char input[BUFFER];
	char output[BUFFER];
	int len;
	
	if((sock = socket(AF_INET, SOCK_STREAM,0))==-1)
	{
		perror("Socket:");
		exit(-1);
	}
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(atoi(argv[2]));
	remote_server.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&remote_server.sin_zero,8);
	
	if((connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in)))==-1)
	{
		perror("bind: ");
		exit(-1);
	}
	//recieve welcome and handshake
	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s\n",output);
	send(sock,"HANDSHAKE",BUFFER,0);
	
	//sending alias and handshaking 
	char ali[100];
	printf("ALIAS : ");
	cin >> ali;
	send(sock, ali, strlen(ali),0);
	recv(sock,output,BUFFER,0);
	
	//sending server of client port and handshaking
	char sprt[10];
	sprintf(sprt, "%d", sport);
	send(sock, sprt, strlen(sprt),0);
	recv(sock,output,BUFFER,0);
	
	//sending first value and rec the handshake
	cin >> input;
	send(sock, input, strlen(input),0);
	recv(sock, output ,strlen(input),0);
	
	while(1)
{
		//rec the result of the value sent CASE 1,2,3 will work
		len = recv(sock, output, BUFFER, 0);
		output[len] = '\0';
		printf("%s\n",output);
		cin >> input;
		if(strcmp(input,"4")==0)
		{
			send(sock, input, strlen(input),0);
			char go[100] ;
			printf("FILENAME: ");
			cin >> go;
			send(sock,go,strlen(go),0);
			
			//recv(sock,go,BUFFER,0);
		}
		else if(strcmp(input,"9")!=0)
		send(sock, input, strlen(input),0);
		//CASE 9:
		else
		{
			send(sock, input, strlen(input),0);
			
			//rec : filename
			char filename[100];
			printf("FILENAME : ");
			cin >> filename;
			strcpy(fname,filename);
			send(sock, filename, strlen(filename),0);
			//recv(sock,output,BUFFER,0);
			
			//rec : Alias
			char alias[100];
			printf("ALIAS : " );
			cin >> alias;
			send(sock, alias, strlen(alias),0);

			
			char ipee[50];
			char port[10];
			
			//rec : ip and handhake
			recv(sock, ipee, BUFFER, 0);
			send(sock,"HANDSHAKE",BUFFER,0);
			//rec : port and handshake
			recv(sock, port, BUFFER, 0);
			send(sock,"HANDSHAKE",BUFFER,0);
			
			downloadfunc(ipee,port,filename);
		}
	}
	close(sock);
}
	return 0;
}
