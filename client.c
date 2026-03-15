#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;

// Thread to receive messages from server
void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];

    while(1)
    {
        int len = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if(len <= 0)
        {
            printf("Disconnected from server\n");
            close(sock);
            exit(0);
        }

        buffer[len] = '\0';
        printf("%s", buffer);
    }

    return NULL;
}

int main()
{
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char username[50];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Server configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to chat server\n");

    // Ask for username
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);

    // Remove newline
    username[strcspn(username, "\n")] = 0;

    // Send username to server
    send(sock, username, strlen(username), 0);

    // Create thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // Send messages
    while(1)
    {
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}