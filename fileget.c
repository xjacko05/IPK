#include <stdio.h> 
#include <netdb.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <ctype.h>




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
            serv_file = malloc(((int)strlen(serv_path) - i) * sizeof(char));
            memcpy(serv_file, &serv_path[i+1], ((int)strlen(serv_path) - i - 1));
            serv_file[(int)strlen(serv_path) - i] = '\0';

        }
    }
    //printf("%s-%s\n", serv_name, serv_file);
    //printf("%li-%li-%li\n", strlen(serv_name), strlen(serv_file), strlen(serv_path));
    
    
    
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

    char* tr_msg = calloc(9+strlen(serv_name), sizeof(char));
    strcpy(tr_msg, "WHEREIS ");
    strcat(tr_msg, serv_name);
    //for (int i = 0; i < (int)strlen(msg); i++){
    //    printf("%c-%i\n", msg[i], i);
    //}
    

    
    //char reply1[2000];
    //memset(reply1, 'a', 2000);
    char* tr_reply = calloc(2000, sizeof(char));
    char reply2[2000];

    send(sockfd, tr_msg, strlen(tr_msg), 0);
    sleep(1);
    recv(sockfd, tr_reply, 2000 , 0);

    char* fl_ip_port = &tr_reply[3];
    char* fl_ip;
    int fl_port;

    for (int i = 0; i < (int)strlen(fl_ip_port); i++){
        if (fl_ip_port[i] == ':'){
            fl_ip = malloc((i+1) * sizeof(char));
            memcpy(fl_ip, fl_ip_port, i);
            fl_ip[i] = '\0';
            fl_port = atoi(&fl_ip_port[i+1]);
        }
    }
    //printf("%s-%i\n", fl_ip, fl_port);
    //printf("%li\n", strlen(fl_ip));


    int fl_sockfd; 
    struct sockaddr_in fl_servaddr; 
  
    // socket create and varification 
    fl_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (fl_sockfd == -1){ 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Socket successfully created..\n");
    }
    bzero(&fl_servaddr, sizeof(fl_servaddr)); 
  
    // assign IP, PORT 
    fl_servaddr.sin_family = AF_INET; 
    fl_servaddr.sin_addr.s_addr = inet_addr(fl_ip); 
    fl_servaddr.sin_port = htons(fl_port); 
  
    // connect the client socket to server socket 
    if (connect(fl_sockfd, (struct sockaddr *)&fl_servaddr, sizeof(fl_servaddr)) != 0){ 
        printf("connection with the server failed...\n"); 
        exit(0);
    } 
    else{
        printf("connected to the server..\n"); 
    }


    char* fl_msg = calloc(46+strlen(serv_name)+strlen(serv_file), sizeof(char));
    sprintf(fl_msg, "GET %s FSP/1.0\r\nHostname: %s\r\nAgent: xjacko05\r\n\r\n", serv_file ,serv_name);
    //for (int i = 0; i < (int)strlen(fl_msg); i++){
    //    printf("%i\t%c\n", i, fl_msg[i]);
    //}
    send(fl_sockfd, fl_msg, strlen(fl_msg), 0);
    sleep(1);

    FILE *sockfile = (FILE *) fdopen(fl_sockfd, "r");
    char g = fgetc(sockfile);
    int length = 0;
    while (g != '\n'){
        //printf("%i\t%c\n", g, g);
        g = fgetc(sockfile);
    }
    while (!isdigit(g)){
        g = fgetc(sockfile);
    }
    while (isdigit(g)){
        length = (length*10) + (g-48);
        g = fgetc(sockfile);
    }
    //printf("LENGTH=%i\n", length);
    g = fgetc(sockfile);
    g = fgetc(sockfile);
    g = fgetc(sockfile);
    g = fgetc(sockfile);

    FILE* result = fopen(serv_file, "w");
    for (int i = 0; i < length; ++i){
        putc(g, result);
        g = fgetc(sockfile);
    }


    
    //for (int i = 0; i < 3; i++){
    //    printf("%i %c\n", i, fgetc(sockfile));
    //}
    //fgets(reply1, 7, sockfile);
    //printf("%s\n%li\n", reply1, strlen(reply1));
    
    //recv(sockfd, reply2, 5 , 0);
    //fgets(reply2, 10, sockfile);
    //printf("%s\n", reply2);
  
    // close the socket 
    close(sockfd);
    close(fl_sockfd);
    fclose(sockfile);
    fclose(result);
}