//
// Created by xraw on 26.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
//#include <string.h>
#include <signal.h>

#include "helper.h"

int client_queue;
int server_queue;
struct msg client_msg;
struct msg server_msg;
pid_t pid;
pid_t child;
int client_id;


void msgsender();

void msgreceiver();

void print_message();

void msg_stop();

void msg_list();

void msg_2all();

void msg_2one();

void error(char *message);

void delete_queue();

void sigint_handler();

void set_sigint_handler();


int main() {
    pid = getpid();
    set_sigint_handler();

    if ((client_queue = msgget(IPC_PRIVATE, Q_PERMISSION)) == -1) {
        error("Error while creating client queue");
    }
    if (atexit(delete_queue) != 0) error("Cannot set exit function");

    printf("[client] client_queue = %d\n", client_queue);

    key_t server_key = get_server_key();
    if ((server_queue = msgget(server_key, 0)) == -1) {
        error("Error while accessing server queue");
    }

    printf("[client] server_queue = %d\n", server_queue);

    time_t t = time(NULL);
    client_msg.mtext.time = *localtime(&t);
    client_msg.mtype = MSG_INIT;
    client_msg.mtext.id = pid;
    client_msg.mtext.info = client_queue;
    if (msgsnd(server_queue, &client_msg, sizeof(client_msg.mtext), 0) == -1) {
        error("Error while sending INIT message");
    }

    if (msgrcv(client_queue, &server_msg, sizeof(server_msg.mtext), MSG_INIT, 0) == -1) {
        error("Error while receiving message");
    }

    if (server_msg.mtext.id == -1) {
        printf("[client] Maximum number of clients reached!\n");
        exit(EXIT_SUCCESS);
    }
    else {
        client_id = server_msg.mtext.id;
        printf("[client] Successfully connected to server; client id: %d\n", client_id);
    }

    if ((child = fork()) == 0) {
        msgreceiver();
    }
    else {
        msgsender();
    }
}


void msgsender() {
    char str[1100];
    char str_type[1100];
    char *token;
    char *text;

    while (1) {
        strcpy(str_type, "");

        printf("> ");
        fgets(str, 1099, stdin);
        sscanf(str, "%s", str_type);

        long type = string_to_mtype(str_type);

        switch (type) {
            case MSG_STOP:
                kill(child, SIGINT);
                msg_stop();
                break;
            case MSG_LIST:
                msg_list();
                break;
            case MSG_2ALL:
                strtok_r(str, " ", &text);
                msg_2all(text);
                break;
            case MSG_2ONE:
                strtok_r(str, " ", &text);
                token = strtok_r(text, " ", &text);
                if (token != NULL && atoi(token) != 0) {
                    msg_2one(text, atoi(token));
                }
                else {
                    printf("[warning] Wrong 2ONE client_id!\n");
                }
                break;
            default:
                break;
        }
    }
}


void msgreceiver() {
    while (1) {
        if (msgrcv(client_queue, &server_msg, sizeof(server_msg.mtext), -MSG_MESSAGE, 0) == -1) {
            error("Error while receiving message");
        }

        switch (server_msg.mtype) {
            case MSG_STOP:
                printf("Server stop\n");
                kill(pid, SIGINT);
                exit(EXIT_SUCCESS);
            case MSG_2ALL:
            case MSG_2ONE:
                printf("New message arrived\n");
                print_message();
                break;
        }
    }
}


void print_message() {
    struct tm *tm = &server_msg.mtext.time;
    if (server_msg.mtype == MSG_2ALL) {
        printf(">>> [%d-%02d-%02d %02d:%02d:%02d]\n>>> From: %d, To: ALL\n>>> Message: %s\n> ", tm->tm_year + 1900, tm->tm_mon + 1,
               tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, server_msg.mtext.id, server_msg.mtext.text);
    }
    else {
        printf(">>> [%d-%02d-%02d %02d:%02d:%02d]\n>>> From: %d, To: %d\n>>> Message: %s\n> ", tm->tm_year + 1900, tm->tm_mon + 1,
               tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, server_msg.mtext.id, server_msg.mtext.info, server_msg.mtext.text);
    }
    fflush(stdout);
}


void msg_stop() {
    time_t t = time(NULL);
    client_msg.mtext.time = *localtime(&t);
    client_msg.mtype = MSG_STOP;
    client_msg.mtext.id = client_id;
    client_msg.mtext.info = 0;
    strcpy(client_msg.mtext.text, "");
    if (msgsnd(server_queue, &client_msg, sizeof(client_msg.mtext), 0) == -1) {
        error("Error while sending STOP message");
    }
    exit(EXIT_SUCCESS);
}


void msg_list() {
    time_t t = time(NULL);
    client_msg.mtext.time = *localtime(&t);
    client_msg.mtype = MSG_LIST;
    client_msg.mtext.id = client_id;
    client_msg.mtext.info = 0;
    strcpy(client_msg.mtext.text, "");
    if (msgsnd(server_queue, &client_msg, sizeof(client_msg.mtext), 0) == -1) {
        error("Error while sending LIST message");
    }
}


void msg_2all(char *msg) {
    msg[strlen(msg) - 1] = '\0';
    time_t t = time(NULL);
    client_msg.mtext.time = *localtime(&t);
    client_msg.mtype = MSG_2ALL;
    client_msg.mtext.id = client_id;
    client_msg.mtext.info = 0;
    strncpy(client_msg.mtext.text, msg, MAX_TEXT_LEN - 1);
    if (msgsnd(server_queue, &client_msg, sizeof(client_msg.mtext), 0) == -1) {
        error("Error while sending LIST message");
    }
}


void msg_2one(char *msg, int receiver_id) {
    msg[strlen(msg) - 1] = '\0';
    time_t t = time(NULL);
    client_msg.mtext.time = *localtime(&t);
    client_msg.mtype = MSG_2ONE;
    client_msg.mtext.id = client_id;
    client_msg.mtext.info = receiver_id;
    strncpy(client_msg.mtext.text, msg, MAX_TEXT_LEN - 1);
    if (msgsnd(server_queue, &client_msg, sizeof(client_msg.mtext), 0) == -1) {
        error("Error while sending LIST message");
    }
}


void error(char *message) {
    if (errno != 0) {
        perror(message);
    } else {
        fprintf(stderr, "%s\n", message);
    }
    exit(EXIT_FAILURE);
}


void delete_queue() {
    if (pid == getpid()) {
        printf("\n[delete_queue]\n");
        msgctl(client_queue, IPC_RMID, NULL);
    }
}


void sigint_handler() {
    if (pid == getpid()) {
        msg_stop();
    }
    exit(EXIT_SUCCESS);
}


void set_sigint_handler() {
    sigset_t mask;
    if (sigfillset(&mask) == -1) error("sigfillset error");
    if (sigdelset(&mask, SIGINT) == -1) error("sigdelset error");

    struct sigaction action;
    action.sa_handler = sigint_handler;
    action.sa_mask = mask;
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) == -1) error("sigaction error");
}

