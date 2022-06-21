//
// Created by xraw on 03.06.2022.
//

#include "helper.h"


int socket_local;
int socket_network;
struct client_info *clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void error(char *message);

void free_clients();

int local_socket(char *path);

int network_socket(char *port);

int poll_get_fd();

void print_clients();

void send_message(int fd, int msg_type, char* data);

void add_client(char *client_name, int client_fd);

void remove_client(int fd);

void pong(char *client_name);

void move(char *client_name, char *data);

void game_end(char *client_name, char *data);

void *ping();

void server_loop();


int main(int argc, char* argv[]) {
    if (argc != 3) {
        error("Two arguments expected");
    }

    clients = calloc(MAX_CLIENTS, sizeof(struct client_info));
    atexit(free_clients);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].fd = -1;
        clients[i].opponent = -1;
        clients[i].active = -1;
    }

    socket_local = local_socket(argv[2]);
    socket_network = network_socket(argv[1]);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    server_loop();
}


void server_loop() {
    while (1) {
        print_clients();

        int fd = poll_get_fd();
        printf("[server] fd = %d\n", fd);

        char buffer[MSG_LEN + 1];
        if (recv(fd, buffer, MSG_LEN, 0) == 0) {
            printf("[server] Disconnected client: %d\n", fd);
            remove_client(fd);
            continue;
        }

        char *msg_type = strtok(buffer, "#");
        char *client_name = strtok(NULL, "#");
        char *data = strtok(NULL, "#");

        printf("[server] msg_type = %s\n", msg_type);
        printf("[server] client_name = %s\n", client_name);
        printf("[server] data = %s\n", data);

        pthread_mutex_lock(&clients_mutex);
        switch (atoi(msg_type)) {
            case ADD_CLIENT:
                add_client(client_name, fd);
                break;
            case PONG:
                pong(client_name);
                break;
            case MOVE:
                move(client_name, data);
                break;
            case END_GAME:
                game_end(client_name, data);
                break;
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}


void add_client(char *client_name, int client_fd) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd != -1 && strcmp(clients[i].name, client_name) == 0) {
            send_message(client_fd, EXISTING_NAME, "");
            return;
        }
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd == -1 && (i % 2) == 0) {
            strcpy(clients[i].name, client_name);
            clients[i].fd = client_fd;

            send_message(client_fd, NO_OPPONENT, "");

            return;
        }
        else if (clients[i].fd == -1) {
            strcpy(clients[i].name, client_name);
            clients[i].fd = client_fd;
            clients[i].opponent = i - 1;
            clients[i].active = 1;
            clients[i - 1].opponent = i;
            clients[i - 1].active = 1;

            send_message(client_fd, START_GAME_O, clients[i - 1].name);
            send_message(clients[i - 1].fd, START_GAME_X, client_name);

            return;
        }
    }

    send_message(client_fd, FULL_SERVER, "");
}


void remove_client(int fd) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd == fd) {
            int j = clients[i].opponent;

            if (j != -1) {
                send_message(clients[j].fd, OPPONENT_DISCONNECTED, "");

                close(clients[j].fd);
                clients[j].fd = -1;
                strcpy(clients[j].name, "");
                clients[j].opponent = -1;
                clients[j].active = -1;
            }
            close(clients[i].fd);
            clients[i].fd = -1;
            strcpy(clients[i].name, "");
            clients[i].opponent = -1;
            clients[i].active = -1;

            break;
        }
    }
}


void pong(char *client_name) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd != -1 && strcmp(clients[i].name, client_name) == 0) {
            printf("[pong] pong from client: %s\n", client_name);
            clients[i].active = 1;
        }
    }
}


void move(char *client_name, char *data) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd != -1 && strcmp(clients[i].name, client_name) == 0) {
            send_message(clients[clients[i].opponent].fd, MOVE, data);
            break;
        }
    }
}


void game_end(char *client_name, char *data) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd != -1 && strcmp(clients[i].name, client_name) == 0) {
            send_message(clients[i].fd, END_GAME, data);
            send_message(clients[clients[i].opponent].fd, END_GAME, data);
            break;
        }
    }
}


int poll_get_fd() {
    struct pollfd *poll_fds;

    poll_fds = calloc(MAX_CLIENTS + 2, sizeof(struct pollfd));

    if (poll_fds == NULL) error("Poll fds calloc error");

    poll_fds[0].fd = socket_local;
    poll_fds[0].events = POLLIN;
    poll_fds[1].fd = socket_network;
    poll_fds[1].events = POLLIN;

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd != -1) {
            poll_fds[i + 2].fd = clients[i].fd;
            poll_fds[i + 2].events = POLLIN;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    poll(poll_fds, MAX_CLIENTS + 2, -1);

    int fd;
    for (int i = 0; i < MAX_CLIENTS + 2; ++i) {
        if (poll_fds[i].revents & POLLIN) {
            if (i < 2) {
                fd = accept(poll_fds[i].fd, NULL, NULL);
            } else {
                fd = poll_fds[i].fd;
            }

            free(poll_fds);
            return fd;
        }
    }

    free(poll_fds);
    return -1;
}


int local_socket(char *path) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) error("Error while creating socket");

    struct sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    unlink(path);

    if (bind(fd, (struct sockaddr *) &address, sizeof(address)) < 0) error("Local socket bind failed");

    if (listen(fd, MAX_CLIENTS) < 0) error("Local socket listen failed");

    printf("[local_socket] fd = %d\n", fd);

    return fd;
}


int network_socket(char *port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) error("Error while creating socket");

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(atoi(port));


    if (bind(fd, (struct sockaddr *) &address, sizeof(address)) < 0) error("Network socket bind failed");

    if (listen(fd, MAX_CLIENTS) < 0) error("Local socket listen failed");

    printf("[network_socket] fd = %d\n", fd);

    return fd;
}


void print_clients() {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        printf("%d: { fd: %d, name: %s, opponent: %d, active: %d }\n", i, clients[i].fd, clients[i].name, clients[i].opponent, clients[i].active);
    }
}


void send_message(int fd, int msg_type, char* data) {
    char buffer[MSG_LEN];
    sprintf(buffer, "%d#%s", msg_type, data);
    send(fd, buffer, MSG_LEN, MSG_NOSIGNAL);
}


void *ping() {
    while (1) {
        sleep(10);
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].fd != -1) {

                if (clients[i].active == 1) {
                    clients[i].active = 0;
                    send_message(clients[i].fd, PING, "");
                }
                else if (clients[i].active == 0) {
                    printf("[ping] Lost connection with client: %s\n", clients[i].name);
                    remove_client(clients[i].fd);
                }

            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}


void free_clients() {
    free(clients);
}


void error(char *message) {
    if (errno != 0) {
        perror(message);
    }
    else {
        fprintf(stderr, "[error] %s\n", message);
    }
    exit(EXIT_FAILURE);
}