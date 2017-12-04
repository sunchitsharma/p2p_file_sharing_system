#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <time.h>




using namespace std;
struct S {
  char aname[100];
  char connip[100];
  char nof[100];
  int port;
  int sport;
};
	struct sockaddr_in server, client;

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

#define ERROR -1
#define MAX_CLIENTS 2
#define MAX_DATA 1024
#define BUFFER 1024
vector<S> mainvec;
int csize=0;

void *threadhandler(void *socket_desc)
{
		
	
		char data[1024]={0};
		char currals[100];
		char csport[100];
		int datalen =1;
		int cli = *(int*)socket_desc;
		
		//sending welcome and handshaking
		char message[]= "==== WELCOME ====\nPress 2 to Share\nPress 3 to Deregister\nPress 4 to search\nPress 5 to Get Entire List\nPress 9 to get";
		int sent = send(cli, message, strlen(message),0);
		recv(cli, data, MAX_DATA, 0);
		
	
		//rec : alais of client + Handshake
		char data1[1024];
		recv(cli, data1, 1024, 0);
		send(cli,"HANDSHAKE1",BUFFER,0);
		//data[sizeof(data1)]='\0';
		strcpy(currals, data1);
		printf("%s",currals);
		
		//rec : server port no. of client + Handshake
		char data2[1024];
		recv(cli, data2, 1024, 0);
		send(cli,"HANDSHAKE2",BUFFER,0);
		//data[strlen(data2)]='\0';
		strcpy(csport, data2);
		

		int var=0;

		while(datalen)
		{
			strcpy(data,"\0");
			
			//rec: menu value and handshaking
			datalen = recv(cli, data, MAX_DATA, 0);
			//send(cli,"HANDSHAKE3",BUFFER,0);
	
			
			printf("\nNUMBER : %d\n",strcmp(data,"1"));
			
			
			if(datalen)
			{

				//case 5 : 
				if(strcmp(data,"1")==4)
				{
					FILE *repof = fopen("repo.txt","aw+");
					printf("REACHED");
					char retarray[1024]={0};
					for(int i=0;i<csize;++i)
					{
						printf("\n\n -> NAME : %s  IP : %s PORT : %d  ALAIS : %s S.PORT :%d",mainvec[i].nof,mainvec[i].connip,mainvec[i].port,mainvec[i].aname,mainvec[i].sport);
						
						fprintf(repof,"%s ** %s:%d ** %s\n",mainvec[i].nof,mainvec[i].connip,mainvec[i].port,mainvec[i].aname);
						
						strcat(retarray,mainvec[i].nof);
						strcat(retarray," : ");
						strcat(retarray,mainvec[i].aname);
						strcat(retarray,"\n");
						
					}
					fclose(repof);
					send(cli , retarray, 1024,0);
				}
				if(strcmp(data,"1")==3)
				{
					char newdat[100]={0};
					recv(cli, newdat, MAX_DATA, 0);
					printf("%s",newdat);
					//send(cli,"HANDSHAKESHARE",MAX_DATA,0);
					FILE *repof = fopen("repo.txt","aw+");
					printf("REACHED");
					char retarray[1024]={0};
					for(int i=0;i<csize;++i)
					{	
						if(startsWith(newdat,mainvec[i].nof))
						{strcat(retarray,mainvec[i].nof);
						strcat(retarray," : ");
						strcat(retarray,mainvec[i].aname);
						strcat(retarray,"\n");}
						
					}
					fclose(repof);
					send(cli , retarray, 1024,0);
				}
				//case 2 :		
				else if(strcmp(data,"1")==1)
				{
					
					char dummy[100]="FILENAME2 : ";
					send(cli , dummy, strlen(dummy),0);
					char temp[100];
					int x=recv(cli, temp, MAX_DATA, 0);
					temp[x]='\0';
					FILE *logf = fopen("log.txt","aw+");
					fprintf(logf,"%lu : SHARE : %s\n",(unsigned long)time(NULL),inet_ntoa(client.sin_addr));
					fclose(logf);
					
					if(temp != NULL)
					{
						S ptr;
						strcpy(ptr.aname,currals);
						strcpy(ptr.nof,temp);
						ptr.port=(ntohs(client.sin_port));
						strcpy(ptr.connip,inet_ntoa(client.sin_addr));
						ptr.sport=(atoi(csport));
						mainvec.push_back(ptr);
					//	printf("\n\n -> NAME : %s ALAIS : %s %d  %s",mainvec[csize].nof,mainvec[csize].connip,mainvec[csize].port,mainvec[csize].aname);
						csize++;
						char dummy[100]="DONE! \n ";
						send(cli , dummy, strlen(dummy),0);		
					}
					else
					{
						char dummy[]="SORRY NO NAME ENTERED! \n ";
						send(cli , dummy, strlen(dummy),0);		
					}
				}
				//CASE 3 :
				if(strcmp(data,"1")==2)
				{
					char dummy[]="FILENAME3 : ";
					send(cli , dummy, strlen(dummy),0);
					char temp[100];
					int x=recv(cli, temp, MAX_DATA, 0);
					temp[x]='\0';
					FILE *logf = fopen("log.txt","aw+");
					fprintf(logf,"%lu : DEREGISTER : %s\n",(unsigned long)time(NULL),inet_ntoa(client.sin_addr));
					fclose(logf);
					if(temp != NULL)
					{
						int i=0;
						while((strcmp(temp,mainvec[i].nof)!=0)&&i<csize)
						{
							
							i++;
						}
						if(i<csize)
						{
							mainvec.erase(mainvec.begin()+i);
							char dummy[]="REMOVED FROM CRS";
							send(cli , dummy, strlen(dummy),0);
							
						}
						else
						{
							char dummy[]="COULD NOT FIND THE GIVEN FILE";
							send(cli , dummy, strlen(dummy),0);
							
						}
						
					}
						
				}
				if(strcmp(data,"1")==8)
				{
					FILE *logf = fopen("log.txt","aw+");
					fprintf(logf,"%lu : DOWNLOAD REQUEST : %s\n",(unsigned long)time(NULL),inet_ntoa(client.sin_addr));
					fclose(logf);
					//Rec : Filename and Handshaking
					char fn[100];
					int x=recv(cli, fn, MAX_DATA, 0);
					//send(cli, "HANDSHAKE",BUFFER,0);
					fn[x]='\0';
					printf("RECV : %s\n",fn);
					
					//REC : ALias and NOT Handhaking
					char al[100];
					int y=recv(cli, al, MAX_DATA, 0);
					al[y]='\0';
					printf("RECV : %s",al);
					
					
					int i=0;
					while((strcmp(fn,mainvec[i].nof)!=0))
						{
							
							i++;
						}
						if(i<csize)
						{
							//send the ip and recieve handhake
							char *dummy=mainvec[i].connip;
							printf("SENDING IP : %s",dummy);
							send(cli , dummy, strlen(dummy),0);
							recv(cli, data, BUFFER,0);

							//send the server port and recieve handshake
							char p[10];
							sprintf(p, "%d", mainvec[i].sport);
							printf("SENDING PORT : %s",p);
							send(cli , p, strlen(p),0);
							recv(cli, data, BUFFER,0);
							
							
						}
						else
						{
							char dummy[]="0";
							send(cli , dummy, strlen(dummy),0);
							send(cli , dummy, strlen(dummy),0);
							
							
						}

					
				}

			}
		}
		
}

int main(int argc ,char **argv)
{
	int sock,cli;
	//int sent;

	//char message[]= "==== WELCOME ====\nPress 1 to Search\nPress 2 to Register\nPress 3 to Deregister";
	
	if((sock = socket(AF_INET, SOCK_STREAM,0))==-1)
	{
		perror("Socket:");
		exit(-1);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero,8);
	
	unsigned int len=sizeof(struct sockaddr_in);
	
	if((bind(sock, (struct sockaddr *)&server, len))==-1)
	{
		perror("bind: ");
		exit(-1);
	}
	
	if((listen(sock,5))==-1)
	{
		perror("listen :");
		exit(-1);
	}
	
	 while((cli=accept(sock,(struct sockaddr*)&client,&len)))
       {
        printf("New Client connected from port : %d and ip : %s\n",ntohs(client.sin_port), inet_ntoa(client.sin_addr));

        pthread_t sniffer_thread;
        int* new_sock = (int*)malloc(1);
        *new_sock = cli;

        if( pthread_create( &sniffer_thread , NULL ,  threadhandler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (cli < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
	

