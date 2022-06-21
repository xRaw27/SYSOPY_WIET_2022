//
// Created by xraw on 03.06.2022.
//

#include "helper.h"


int socket_server;
int client_sign;
char *name;
int board[9];


void error(char *message);

void send_message(int msg_type, char* data);

void local_connection(char *path);

void network_connection(char *port);

int listen_for_game_start();

void game();

void init_board();

void print_board();

int check_win();

int check_game_status();

void end_game(char *data);

void *move(void *arg);


int main(int argc, char* argv[]) {
    if (argc != 4) {
        error("Two arguments expected");
    }

    name = argv[1];

    if (strchr(name, '#') != NULL) {
        error("Name cannot contain '#' char");
    }

    if (strcmp(argv[2], "local") == 0) {
        printf("[main] Local connection\n");
        local_connection(argv[3]);
        send_message(ADD_CLIENT, "");
    }
    else if (strcmp(argv[2], "network") == 0) {
        printf("[main] Network connection\n");
        network_connection(argv[3]);
        send_message(ADD_CLIENT, "");
    }
    else {
        error("Wrong connection type");
    }

    int game_start_status = listen_for_game_start();
    if (game_start_status != -1) {
        client_sign = game_start_status;
        game();
    }
}


void game() {
    if (client_sign == O_SIGN) {
        printf("Starting game, player sign: O\n");
    }
    else {
        printf("Starting game, player sign: X\n");
    }

    init_board();
    print_board();

    if (client_sign == O_SIGN) {
        int x[1] = {-1};
        pthread_t move_thread;
        pthread_create(&move_thread, NULL, move, (void *)x);
        pthread_detach(move_thread);
    }

    char buffer[MSG_LEN];
    char *msg_type;
    char *data;

    while (1) {
        recv(socket_server, buffer, MSG_LEN, 0);
        msg_type = strtok(buffer, "#");
        data = strtok(NULL, "#");

        switch (atoi(msg_type)) {
            case MOVE: ;
                int x[1] = {atoi(data)};
                pthread_t move_thread;
                pthread_create(&move_thread, NULL, move, (void *)x);
                pthread_detach(move_thread);
                break;

            case PING:
                send_message(PONG, "");
                break;

            case OPPONENT_DISCONNECTED:
                printf("OPPONENT DISCONNECTED\n");
                return;

            case END_GAME:
                end_game(data);
                return;
        }

    }
}


void *move(void *arg) {
    int *enemy_move = (int *)arg;

    if (*enemy_move != -1) {
        board[*enemy_move] = 3 - client_sign;
        print_board();
    }

    int status = check_game_status();
    if (status != 0) {
        char buffer[2];
        sprintf(buffer, "%d", status);
        send_message(END_GAME, buffer);
        pthread_exit(NULL);
    }

    printf("Your move (number 1-9): \n");

    int player_move;
    scanf("%d", &player_move);
    while (player_move < 1 || player_move > 9 || board[player_move - 1] != 0) {
        printf("Illegal move, try again!\n");
        scanf("%d", &player_move);
    }
    --player_move;
    board[player_move] = client_sign;
    print_board();

    char buffer[2];
    sprintf(buffer, "%d", player_move);
    send_message(MOVE, buffer);
    pthread_exit(NULL);
}


int listen_for_game_start() {
    char buffer[MSG_LEN + 1];

    recv(socket_server, buffer, MSG_LEN, 0);
    char *msg_type = strtok(buffer, "#");
    char *data = strtok(NULL, "#");
    int type = atoi(msg_type);

    if (type == EXISTING_NAME) {
        printf("Name already exists\n");
        return -1;
    }
    if (type == FULL_SERVER) {
        printf("Full server\n");
        return -1;
    }
    if (type == NO_OPPONENT) {
        printf("Waiting for opponent...\n");
        return listen_for_game_start();
    }
    if (type == START_GAME_O) {
        printf("OPPONENT NAME: %s\n", data);
        return O_SIGN;
    }
    if (type == START_GAME_X) {
        printf("OPPONENT NAME: %s\n", data);
        return X_SIGN;
    }

    return  -1;
}


void init_board() {
    for (int i = 0; i < 9; ++i) {
        board[i] = 0;
    }
}


void print_board() {
    printf("\n");
    for (int i = 0; i < 9; ++i) {
        switch (board[i]) {
            case O_SIGN:
                printf(" O ");
                break;
            case X_SIGN:
                printf(" X ");
                break;
            default:
                printf("   ");
        }

        if (i == 2 || i == 5) {
            printf("\n---|---|---\n");
        }
        else if (i != 8) {
            printf("|");
        }
    }
    printf("\n");
}


int check_win() {
    for (int i = 0; i < 3; ++i) {
        if (board[3 * i] != 0 && board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2]) return board[3 * i];
        if (board[i] != 0 && board[i] == board[i + 3] && board[i] == board[i + 6]) return board[i];
    }
    if (board[0] != 0 && board[0] == board[4] && board[0] == board[8]) return board[0];
    if (board[2] != 0 && board[2] == board[4] && board[2] == board[6]) return board[2];
    return 0;
}


int check_game_status() {
    int status = check_win();
    if (status == 0) {
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) return 0;
        }
        status = DRAW;
    }
    return status;
}


void end_game(char *data) {
    int status = atoi(data);

    switch (status) {
        case O_SIGN:
        case X_SIGN:
            if (status == client_sign) {
                printf("\n *** VICTORY :) *** \n");
            }
            else {
                printf("\n *** DEFEAT :( *** \n");
            }
            break;

        case DRAW:
            printf("\n *** DRAW *** \n");
            break;

        default:
            break;
    }
}


void local_connection(char *path) {
    socket_server = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_server < 0) error("Server socket failed");

    struct sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    if (connect(socket_server, (struct sockaddr *) &address, sizeof(address)) < 0) error("Server socket connection failed");
}


void network_connection(char *port) {
    socket_server = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_server < 0) error("Server socket failed");

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(atoi(port));

    if (connect(socket_server, (struct sockaddr *) &address, sizeof(address)) < 0) error("Server socket connection failed");
}


void send_message(int msg_type, char* data) {
    char buffer[MSG_LEN];
    sprintf(buffer, "%d#%s#%s", msg_type, name, data);
    send(socket_server, buffer, MSG_LEN, 0);
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
