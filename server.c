#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char name[50];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

pthread_mutex_t lock;

// send message to all clients
void broadcast(char *message, int sender_socket)
{
    pthread_mutex_lock(&lock);

    for(int i = 0; i < client_count; i++)
    {
        if(clients[i].socket != sender_socket)
        {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&lock);
}

// thread for each client
void *handle_client(void *arg)
{
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char name[50];

    // receive username first
    int len = recv(client_socket, name, sizeof(name)-1, 0);
    name[len] = '\0';

    pthread_mutex_lock(&lock);
    for(int i=0;i<client_count;i++)
    {
        if(clients[i].socket == client_socket)
        {
            strcpy(clients[i].name, name);
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    printf("%s joined the chat\n", name);

    sprintf(buffer,"*** %s joined the chat ***\n", name);
    broadcast(buffer, client_socket);

    while(1)
    {
        len = recv(client_socket, buffer, BUFFER_SIZE-1, 0);

        if(len <= 0)
        {
            printf("%s left the chat\n", name);

            sprintf(buffer,"*** %s left the chat ***\n", name);
            broadcast(buffer, client_socket);

            close(client_socket);
            break;
        }

        buffer[len] = '\0';

        char message[BUFFER_SIZE];
        sprintf(message,"%s: %s", name, buffer);

        printf("%s", message);

        broadcast(message, client_socket);
    }

    return NULL;
}

int main()
{
    int server_socket;
    struct sockaddr_in server_addr;

    pthread_mutex_init(&lock,NULL);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    listen(server_socket, MAX_CLIENTS);

    printf("Chat server started on port %d\n",PORT);

    while(1)
    {
        int client_socket;

        client_socket = accept(server_socket,NULL,NULL);

        printf("New client connected\n");

        pthread_mutex_lock(&lock);
        clients[client_count].socket = client_socket;
        strcpy(clients[client_count].name,"Unknown");
        client_count++;
        pthread_mutex_unlock(&lock);

        pthread_t tid;
        pthread_create(&tid,NULL,handle_client,&client_socket);
    }

    close(server_socket);
    return 0;
}