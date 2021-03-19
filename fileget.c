#include <stdio.h> 
#include <netdb.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>




int main(int argc, char const *argv[]){

    //arg parse
    const char* ip_port = argv[2];
    char* tr_ip;
    int tr_port;
    
    for (int i = 0; i < (int)strlen(ip_port); i++){
        if (ip_port[i] == ':'){
            tr_ip = malloc((i+1) * sizeof(char));
            memcpy(tr_ip, ip_port, i);
            tr_ip[i] = '\0';
            tr_port = atoi(&ip_port[i+1]);
        }
    }
    //printf("%s-%i\n", tr_ip, tr_port);
    //printf("%li\n", strlen(tr_ip));
    
    const char* serv_path = argv[4];
    char* serv_name;
    char* serv_file;
    
    for (int i = 6; i < (int)strlen(serv_path); i++){
        if (serv_path[i] == '/'){
            serv_name = malloc((i+1-6) * sizeof(char));
            memcpy(serv_name, &serv_path[6], i-6);
            serv_name[i-6+1] = '\0';
            /*for (int ii = i+1; ii < (int)strlen(serv_path); ii++){
                if (serv_path[ii] == '\0'){
                    printf("Hello\n");
                    serv_file = malloc((ii-i) * sizeof(char));
                    memcpy(serv_file, &serv_path[i+1], (ii-(i+1)));
                    serv_file[ii-i] = '\0';
                }
            }*/
            //serv_file = malloc((ii-i) * sizeof(char));
            //memcpy(serv_file, &serv_path[i+1], (ii-(i+1)));
            //serv_file[ii-i] = '\0';
            //int s = 0;
            //for(int e = ++i; e < (int)strlen(serv_path); e++){
            //    printf("%c\n", serv_path[e]);
            //    s = e;
            //}
            serv_file = malloc(((int)strlen(serv_path) - i) * sizeof(char));
            memcpy(serv_file, &serv_path[i+1], ((int)strlen(serv_path) - i - 1));
            serv_file[(int)strlen(serv_path) - i] = '\0';

        }
    }
    printf("%s-%s\n", serv_name, serv_file);
    printf("%li-%li-%li\n", strlen(serv_name), strlen(serv_file), strlen(serv_path));
    
    
    
    int sockfd; 
    struct sockaddr_in servaddr; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (sockfd == -1){ 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(tr_ip); 
    servaddr.sin_port = htons(tr_port); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0){ 
        printf("connection with the server failed...\n"); 
        exit(0);
    } 
    else{
        printf("connected to the server..\n"); 
    }
    // function for chat 
    //func(sockfd); 

    char* msg = "whereis idk";
    char reply1[2000];
    char reply2[2000];

    

    send(sockfd, msg, strlen(msg), 0);
    FILE *sockfile = (FILE *) fdopen(sockfd, "r");

    //recv(sockfd, reply1, 5 , 0);
    fgets(reply1, 7, sockfile);
    printf("%s\n", reply1);
    //recv(sockfd, reply2, 5 , 0);
    fgets(reply2, 10, sockfile);
    printf("%s\n", reply2);
  
    // close the socket 
    close(sockfd);
}