#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

// global vars
pthread_mutex_t mutex;
char msg[500];
int len;
int clients[20];
int n=0;
struct sockaddr_in ServerIp;
pthread_t recvt;
int sock=0 , Client_sock=0;

void del(){
    printf("\nClosing the server...\n");
    int status = remove("list.txt");
    if (status == 0) {
    printf("File deleted successfully !!!\n"); }
    else{
    printf("File already deleted !!!\n"); }
    printf("Bye\n");
    exit(0);
}

void sendtoall(char *msg,int cur_sock){

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < n; i++) {
        if(clients[i] != cur_sock) {
            int ha = send(clients[i],msg,strlen(msg),0); 
            if(ha < 0) {
                printf("sending failure \n");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void sendparticular(char *msg, int cur_sock, char *name){
	char c;
	char data[100][100];
	FILE *fptr = fopen("list.txt","r");
	int i=-1; int j=0;
	while((c = fgetc(fptr)) != EOF){
		if(c == ' ')
		{	i++; j=0; }
		else{
		data[i][j]=c;
		j++; }
	}
	fclose(fptr);
	
//	printf("This is name %s\n",name);

	pthread_mutex_lock(&mutex);
	for(int x=0; x<i; x++){

//		printf("this is data[x] %s\n",data[x]);
		if(strcmp(data[x],name)==0){
			
//			printf("The socket is %c",data[x+1][0]);
			char *end;
			int z = data[x+1][0] - '0';
//			sscanf(data[x+1][0], "%d", &z);
            int yo = send(z,msg,strlen(msg),0);
			if(yo < 0) {
                printf("sending failure \n");
                continue;
            }

		}
	    pthread_mutex_unlock(&mutex);
	}
//	printf("msg: %s   cur_sock: %d     name:  %s\n",msg,cur_sock,name);
}

void *recvmg(void *client_sock){
    int sock = *((int *)client_sock);
    while((len = recv(sock,msg,500,0)) > 0) {
        msg[len] = '\0';
		char copy[100];
		strcpy(copy,msg);
			///////////////////////////////////////////////		edit the messg
	char *token = strtok(msg, " ");
	token = strtok(NULL, " ");

    
		if(strcmp("ALL",token)==0)
        	sendtoall(copy,sock);
		else{
			sendparticular(copy,sock,token);
		}
		
    }
    
}

int main(){

    FILE *fptr;

    ServerIp.sin_family = AF_INET;
    ServerIp.sin_port = htons(1234);
    ServerIp.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock = socket( AF_INET , SOCK_STREAM, 0 );
    if( bind( sock, (struct sockaddr *)&ServerIp, sizeof(ServerIp)) == -1 ){
        printf("Binding failed, try after sometime... \n");
        exit(0);    }
    else
        printf("Server Started successfully :)\n");
        
    listen( sock ,20 );
        
    while(1){

        signal(SIGINT, del);

        if( (Client_sock = accept(sock, (struct sockaddr *)NULL,NULL)) < 0 )
            printf("accept failed  \n");

		fptr = fopen("list.txt", "a");
		fputs(" ",fptr);
		int ex1 = Client_sock;
		char ex2[5];
		sprintf(ex2,"%d",ex1);
		fputs(ex2,fptr);
		fclose(fptr);
		
		
		printf("This client is in sock %d\n",Client_sock);
        pthread_mutex_lock(&mutex);
        clients[n]= Client_sock;
	//	printf("new client added: %d",clients[n]);
        n++;
        // creating a thread for each client 
        pthread_create(&recvt,NULL,(void *)recvmg,&Client_sock);
        pthread_mutex_unlock(&mutex);
    }
    return 0; 
    
}