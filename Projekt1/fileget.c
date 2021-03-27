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
#include <stdbool.h>



int download(char* fl_ip, int fl_port, char* serv_file, char* serv_name){
//printf("Hello\n");
fprintf(stdout, "\nAttempting download:\nfilename:\t%s\nfrom server:\t%s\nwith ip:port:\t%s:%i\n", serv_file, serv_name, fl_ip, fl_port);
//printf("Hello\n");
    int fl_sockfd;
    struct sockaddr_in fl_servaddr;
  
    // socket create and varification 
    fl_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&fl_servaddr, sizeof(fl_servaddr));
  
    // assign IP, PORT 
    fl_servaddr.sin_family = AF_INET; 
    fl_servaddr.sin_addr.s_addr = inet_addr(fl_ip); 
    fl_servaddr.sin_port = htons(fl_port);
  
    // connect the client socket to server socket 
    if (connect(fl_sockfd, (struct sockaddr *)&fl_servaddr, sizeof(fl_servaddr)) != 0){ 
        fprintf(stderr,"Connection to the file server failed...\n"); 
        exit(0);
    } 
    else{
        fprintf(stdout, "\nconnected to the file server..\n"); 
    }


    char* fl_msg = calloc(46+strlen(serv_name)+strlen(serv_file), sizeof(char));
    sprintf(fl_msg, "GET %s FSP/1.0\r\nHostname: %s\r\nAgent: xjacko05\r\n\r\n", serv_file ,serv_name);
    //for (int i = 0; i < (int)strlen(fl_msg); i++){
    //    printf("%i\t%c\n", i, fl_msg[i]);
    //}
    send(fl_sockfd, fl_msg, strlen(fl_msg), 0);
    sleep(1);

    FILE *sockfile = (FILE *) fdopen(fl_sockfd, "r");
    int length = 0;

    char header[200];
    fgets(header, 200, sockfile);
    //printf("\n%s\n\n", header);
    if (strstr(header, "Not Found") != NULL){
        fprintf(stderr, "\nFile not found\n");
        exit(1);
    }else if (strstr(header, "uccess") == NULL){
        fprintf(stderr, "\nFileserver error\n");
        exit(1);
    }

    char g = fgetc(sockfile);
/*
    while (g != '\n'){
        //printf("%i\t%c\n", g, g);
        g = fgetc(sockfile);
    }
*/
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

    for (int i = 0; i < (int)strlen(serv_file); i++){
        if (serv_file[i] == '/'){
            serv_file[i] = '.';
        }
    }

    FILE* result = fopen(serv_file, "w");
    for (int i = 0; i < length; ++i){
        putc(g, result);
        g = fgetc(sockfile);
    }
    free(fl_msg);
    close(fl_sockfd);
    fclose(sockfile);
    fclose(result);
    fprintf(stdout, "\nFile downloaded sucessfully\n");
    return length;
}


int main(int argc, char const *argv[]){

    //arg parse
    if (argc != 5){
        fprintf(stderr, "Wrong input\n");
        exit(1);
    }
    const char* ip_port;
    const char* serv_path;
    
    if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-f") == 0){
        ip_port = argv[2];
        serv_path = argv[4];
    }else if (strcmp(argv[3], "-n") == 0 && strcmp(argv[1], "-f") == 0){
        ip_port = argv[4];
        serv_path = argv[2];
    }else{
        fprintf(stderr, "Wrong input\n");
        exit(1);
    }
    char* tr_ip = NULL;
    int tr_port = 0;
    char* serv_name = NULL;
    char* serv_file = NULL;

    for (int i = 0; i < (int)strlen(ip_port); ++i){
        if (isdigit(ip_port[i]) == 0 && ip_port[i] != '.' && ip_port[i] != ':'){
            fprintf(stderr, "NAMESERVER containing illegal characters\n");
            exit(1);
        }
    }
    if (serv_path[0] != 'f' || serv_path[1] != 's' || serv_path[2] != 'p' || serv_path[3] != ':' || serv_path[4] != '/' || serv_path[5] != '/'){
        fprintf(stderr, "SURL containing illegal PROTOCOL\n");
        exit(1);
    }
    
    for (int i = 0; i < (int)strlen(ip_port); i++){
        if (ip_port[i] == ':'){
            tr_ip = malloc((i+1) * sizeof(char));
            memcpy(tr_ip, ip_port, i);
            tr_ip[i] = '\0';
            tr_port = atoi(&ip_port[i+1]);
            break;
        }
    }
    //printf("%s-%i\n", tr_ip, tr_port);
    //printf("%li\n", strlen(tr_ip));
    
    
    for (int i = 6; i < (int)strlen(serv_path); i++){
        if (serv_path[i] == '/'){
            serv_name = malloc((i+1-6) * sizeof(char));
            memcpy(serv_name, &serv_path[6], i-6);
            serv_name[i-6+1] = '\0';

            serv_file = malloc(((int)strlen(serv_path) - i) * sizeof(char));
            memcpy(serv_file, &serv_path[i+1], ((int)strlen(serv_path) - i - 1));
            serv_file[(int)strlen(serv_path) - i] = '\0';
            break;

        }else if (serv_path[i] != '.' && serv_path[i] != '-' && serv_path[i] != '_' && isalpha(serv_path[i]) == 0 && isdigit(serv_path[i]) == 0){
            fprintf(stderr, "SURL containing illegal SERV_NAME\n");
            free(tr_ip);
            exit(1);
        }
    }

    if (tr_ip == NULL || serv_file == NULL || serv_name == NULL){
        return 1;
    }
    /*for (int i = (int)strlen(serv_path) - 1; i > 6 ; i--){
        if (serv_path[i] == '/'){
            serv_file = malloc(((int)strlen(serv_path) - i) * sizeof(char));
            memcpy(serv_file, &serv_path[i+1], ((int)strlen(serv_path) - i - 1));
            serv_file[(int)strlen(serv_path) - i] = '\0';
            //printf("O%sO\n",serv_file);
            break;
        }
    }*/

    //printf("%s-%s\n", serv_name, serv_file);
    //printf("%li-%li-%li\n", strlen(serv_name), strlen(serv_file), strlen(serv_path));
    
    
    
    int sockfd; 
    struct sockaddr_in servaddr; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(tr_ip); 
    servaddr.sin_port = htons(tr_port); 
  
    // connect the client socket to server socket 
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    char* tr_msg = calloc(9+strlen(serv_name), sizeof(char));
    strcpy(tr_msg, "WHEREIS ");
    strcat(tr_msg, serv_name);
    //for (int i = 0; i < (int)strlen(msg); i++){
    //    printf("%c-%i\n", msg[i], i);
    //}
    

    

    char* tr_reply = calloc(2000, sizeof(char));

    send(sockfd, tr_msg, strlen(tr_msg), 0);
    sleep(1);
    
    struct timeval timeout;      
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    recv(sockfd, tr_reply, 2000 , 0);


    if (tr_reply[0] == 'E' && tr_reply[1] == 'R' && tr_reply[2] == 'R'){
        fprintf(stderr, "File server not found\n");
        exit(0);
    }
    bool noreply = true;
    for (int i = 0; i < 2000; i++){
        if (tr_reply[i] != 0){
            noreply = false;
        }
    }
    if (noreply){
        fprintf(stderr, "No reply from translate server recieved\n");
        exit(1);
    }




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

    if (strcmp(serv_file, "*") == 0){
    
        int l = download(fl_ip, fl_port, "index", serv_name);
        FILE* index = fopen("index", "r");
        char* line = calloc((l+1), sizeof(char));
        while (fgets(line, (l+1), index) != NULL){
            for(int i = 0; i < l; i++){
                if (line[i] == 13 || line[i] == 10){
                    line[i] = 0;
                }
                //printf("%i\t%c\n", line[i], line[i]);
            }
            download(fl_ip, fl_port, line, serv_name);
            memset(line, 0, l);
        }
        free(line);
    }else{
        download(fl_ip, fl_port, serv_file, serv_name);
    }



    free(fl_ip);
    free(tr_ip);
    free(tr_msg);
    free(tr_reply);
    free(serv_file);
    free(serv_name);

    
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
    return 0;
}