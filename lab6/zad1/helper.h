//
// Created by xraw on 26.04.2022.
//

#ifndef HELPER_H
#define HELPER_H

#define KEY_PROJ_ID 27
#define Q_PERMISSION 0666
#define MAX_TEXT_LEN 1024
#define MAX_NUM_OF_CLIENTS 100

#define MSG_STOP 1
#define MSG_LIST 2
#define MSG_INIT 3
#define MSG_2ONE 4
#define MSG_2ALL 5
#define MSG_MESSAGE 6

#include <string.h>
#include <time.h>

struct msg_text {
    int id;
    int info;
    struct tm time;
    char text[MAX_TEXT_LEN];
};

struct msg {
    long mtype;
    struct msg_text mtext;
};

struct client_info {
    int id;
    int msqid;
    int pid;
};

key_t get_server_key() {
    const char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "getenv error\n");
        exit(EXIT_FAILURE);
    }

    key_t server_key;
    if ((server_key = ftok(home, KEY_PROJ_ID)) == -1) {
        perror("Error while generating server key");
        exit(EXIT_FAILURE);
    }

    return server_key;
}

int string_to_mtype(char *string) {
    if (strcmp(string, "STOP") == 0) {
        return MSG_STOP;
    }
    if (strcmp(string, "LIST") == 0) {
        return MSG_LIST;
    }
    if (strcmp(string, "2ALL") == 0) {
        return MSG_2ALL;
    }
    if (strcmp(string, "2ONE") == 0) {
        return MSG_2ONE;
    }
    return -1;
}

#endif //HELPER_H
