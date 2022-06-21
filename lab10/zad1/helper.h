//
// Created by xraw on 20.06.2022.
//

#ifndef SYSOPY_WIET_2022_HELPER_H
#define SYSOPY_WIET_2022_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/un.h>
#include <poll.h>
#include <pthread.h>
#include "helper.h"

#define ADD_CLIENT 0
#define EXISTING_NAME 1
#define FULL_SERVER 2
#define NO_OPPONENT 3
#define START_GAME_O 4
#define START_GAME_X 5
#define MOVE 6
#define PING 7
#define PONG 8
#define OPPONENT_DISCONNECTED 9
#define END_GAME 10

#define O_SIGN 1
#define X_SIGN 2
#define DRAW 3

#define MSG_LEN 128
#define MAX_NAME_LEN 32
#define MAX_CLIENTS 4

struct client_info {
    char name[MAX_NAME_LEN];
    int fd;
    int opponent;
    int active;
};

#endif //SYSOPY_WIET_2022_HELPER_H
