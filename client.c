
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<stdlib.h>
#include<signal.h>

// global vars
pthread_t recvt;
char send_msg[500];
struct sockaddr_in ServerIp;
char client_name[100];
char msg[500];
int len;
int sock;

void fail(){
    printf("\n Can't connect to socket... \n");
}

void trap(){
    printf("  Nope, not like this, type exit !!!\n");
}

void bhejde(){
    strcpy(send_msg,client_name);
    strcat(send_msg,"-> ");
    strcat(send_msg,msg);
    len = write(sock,send_msg,strlen(send_msg));
}



void *recvmg(void *my_sock)
{
    int sock = *((int *)my_sock);
    int len = recv(sock,msg,500,0);
    while(len > 0) {
        msg[len] = '\0';
        printf(msg);
        len = recv(sock,msg,500,0);
    }
}

int main(int argc,char *argv[]){
    
    strcpy(client_name, argv[1]);
	
    
    FILE *fptr;
	fptr = fopen("list.txt","a");
	fputs(" ", fptr);
	fputs(client_name, fptr);
	fclose(fptr);

    sock = socket( PF_INET, SOCK_STREAM,0);
//	printf("This is new socket allocated: %d\n",sock);
    ServerIp.sin_port = htons(1234);
    ServerIp.sin_family= PF_INET;
    ServerIp.sin_addr.s_addr = inet_addr("127.0.0.1");
    int yo = connect( sock ,(struct sockaddr *)&ServerIp,sizeof(ServerIp));
    if(yo == -1)
        fail();

    pthread_create(&recvt,NULL,(void *)recvmg,&sock);
    
    //keep reading msg
	while(1) {

        signal(SIGINT, trap);
        memset(msg,0,strlen(msg));

        fgets(msg,500,stdin);
//        printf("This is msg:%s",msg);

        if(msg[0]=='e' && msg[1]=='x' && msg[2]=='i' && msg[3]=='t'){
            printf("Exiting, hold on...!!!\n");
            
            char content[100][100];
            int i=0;
            FILE *fp = fopen("list.txt","r");
            char buffer[255];
            fgets(buffer, 255, (FILE*) fp);
            char* token = strtok(buffer, " ");
            while (token != NULL) { 
        	strcat(content[i],token); i++;
            token = strtok(NULL, " "); 
            }
            fclose(fp);
            remove("list.txt");

            int y=0;
            while(y< i){
//	        printf("y= %d content[y]= %s\n",y,content[y]);
	        y++;
            }
//            printf("Client name was: %s\n",client_name);
            FILE *f = fopen("haha.txt","w");
            int x=0; int z=0;
            while(x<i){
                // content[x][0]==client_name[0] && content[x][1]==client_name[1] && content[x][2]==client_name[2]
                if(strcmp(content[x],client_name)==0){
//                    printf("\nMil gaya BC %s\n",content[x]);
                    x=x+2;
                }
                else{
//                    printf("\nNahi gaya BC %s\n",content[x]);
                    fputs(" ",f);
                    fputs(content[x],f);
                    x++;
                }   
            }
            fclose(f);
            rename("haha.txt","list.txt");
            exit(0);
        }

        bhejde();
        memset(msg,0,strlen(msg));
    }
    
    //close the thread
    pthread_join(recvt,NULL);
    close(sock);
    return 0;
}
  