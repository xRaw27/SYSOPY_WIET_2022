//
// Created by xraw on 26.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <mqueue.h>

#include "helper.h"

mqd_t client_queue;
mqd_t server_queue;
char client_queue_name[20];
struct msg client_msg;
struct msg server_msg;
char client_msg_str[MAX_MSG_SIZE];
char server_msg_str[MAX_MSG_SIZE];
pid_t pid;
pid_t child;
int client_id;


void msgsender();

void msgreceiver();

void parse_server_msg_str();

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

    sprintf(client_queue_name, "/queue_%d", pid);
    printf("[client] client_queue_name = %s\n", client_queue_name);

    if ((client_queue = mq_open(client_queue_name, O_RDONLY | O_CREAT, Q_PERMISSION, NULL)) == -1) {
        error("Error while creating client queue");
    }
    if (atexit(delete_queue) != 0) error("Cannot set exit function");

    printf("[client] client_queue = %d\n", client_queue);

    if ((server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        error("Error while accessing server queue");
    }

    printf("[client] server_queue = %d\n", server_queue);

    time_t t = time(NULL);
    sprintf(client_msg_str, "%d_%ld_%d_%d_%s", MSG_INIT, t, pid, 0, "");
    if (mq_send(server_queue, client_msg_str, strlen(client_msg_str) + 1, 1) == -1) {
        error("Error while sending INIT message");
    }

    if (mq_receive(client_queue, server_msg_str, MAX_MSG_SIZE, NULL) == -1) {
        error("Error while receiving message");
    }
    parse_server_msg_str();


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
        if (mq_receive(client_queue, server_msg_str, MAX_MSG_SIZE, NULL) == -1) {
            error("Error while receiving message");
        }
        parse_server_msg_str();

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


void parse_server_msg_str() {
    char *token;

    token = strtok(server_msg_str, "_");
    server_msg.mtype = atol(token);

    token = strtok(NULL, "_");
    time_t t = atol(token);
    server_msg.mtext.time = *localtime(&t);

    token = strtok(NULL, "_");
    server_msg.mtext.id = atoi(token);

    token = strtok(NULL, "_");
    server_msg.mtext.info = atoi(token);

    token = strtok(NULL, "_");
    if (token == NULL) {
        strcpy(server_msg.mtext.text, "");
    }
    else {
        strcpy(server_msg.mtext.text, token);
    }

//    struct tm *tm = &server_msg.mtext.time;
//    printf("[%d-%02d-%02d %02d:%02d:%02d] TYPE: %ld, ID: %d, INFO: %d, TEXT: %s\n",
//           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
//           server_msg.mtype, server_msg.mtext.id, server_msg.mtext.info, server_msg.mtext.text);
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
    sprintf(client_msg_str, "%d_%ld_%d_%d_%s", MSG_STOP, t, client_id, 0, "");
    if (mq_send(server_queue, client_msg_str, strlen(client_msg_str) + 1, 3) == -1) {
        error("Error while sending STOP message");
    }
    exit(EXIT_SUCCESS);
}


void msg_list() {
    time_t t = time(NULL);
    sprintf(client_msg_str, "%d_%ld_%d_%d_%s", MSG_LIST, t, client_id, 0, "");
    if (mq_send(server_queue, client_msg_str, strlen(client_msg_str) + 1, 2) == -1) {
        error("Error while sending LIST message");
    }
}


void msg_2all(char *msg) {
    msg[strlen(msg) - 1] = '\0';
    time_t t = time(NULL);
    sprintf(client_msg_str, "%d_%ld_%d_%d_%s", MSG_2ALL, t, client_id, 0, msg);
    if (mq_send(server_queue, client_msg_str, strlen(client_msg_str) + 1, 1) == -1) {
        error("Error while sending 2ALL message");
    }
}


void msg_2one(char *msg, int receiver_id) {
    msg[strlen(msg) - 1] = '\0';
    time_t t = time(NULL);
    sprintf(client_msg_str, "%d_%ld_%d_%d_%s", MSG_2ONE, t, client_id, receiver_id, msg);
    if (mq_send(server_queue, client_msg_str, strlen(client_msg_str) + 1, 1) == -1) {
        error("Error while sending 2ONE message");
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
        if (mq_unlink(client_queue_name) == -1) {
            fprintf(stderr, "%s\n", "Error while removing server queue");
        }
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

