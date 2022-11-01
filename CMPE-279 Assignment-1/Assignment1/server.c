// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct passwd *pw;
    int childp_status;

    // Show ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Original UID is %d \n", getuid());

    pid_t child_pid = fork();
    if(child_pid<0){
        perror("child failed");
        exit(EXIT_FAILURE);
    }
    else{
        pw = getpwnam("nobody"); // getting UID for nobody
        printf("Forked UID for user nobody is %ld \n", (long)pw->pw_uid);
        if(pw==NULL){
            printf("Get of user information failed.\n");
             exit(1);
        }

        long cp_uid = setuid(pw->pw_uid);

        if(cp_uid == 0)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                (socklen_t*)&addrlen))<0){
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                valread = read( new_socket , buffer, 1024);
                printf("%s\n",buffer );
                send(new_socket , hello , strlen(hello) , 0 );
                printf("Hello message sent\n");
                return 0;
        }
        else
        {
            perror("setuid action failed for user nobody, not child process");
            exit(EXIT_FAILURE);
        }
    }
    if(waitpid(child_pid, &childp_status, 0) < 0){
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    return 0;
}
