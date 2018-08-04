#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


static void foo(){
    printf("hello world\n");

}

int main(){
    printf("hello world\n");
    // int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(1883);
    // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // int ret = connect(socketfd, (struct sockaddr*)&addr, sizeof(addr));
    // printf("%d\n", ret);
    // close(socketfd);

}