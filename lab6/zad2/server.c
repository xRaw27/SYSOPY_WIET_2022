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

mqd_t server_queue;
struct msg client_msg;
struct msg server_msg;
char client_msg_str[MAX_MSG_SIZE];
char server_msg_str[MAX_MSG_SIZE ];
struct client_info clients[MAX_NUM_OF_CLIENTS];
int curr_id;
FILE *fptr;


void parse_client_msg_str();

void execute_client_order();

void msg_stop();

void msg_list();

void msg_init();

void msg_2all();

void msg_2one();

void error(char *message);

void save_log(char *type, int id, const struct tm *tm, const char *msg, const int *receiver_id);

void delete_queue();

void close_log_file();

void sigint_handler();

void set_sigint_handler();

void init_clients_list();


int main() {
    if ((fptr = fopen ("./logfile.log", "a+")) == NULL) {
        error("Error while opening logfile");
    }
    if (atexit(close_log_file) != 0) error("Cannot set exit function");

    fprintf(fptr, "*** SERVER STARTS RUNNING ***\n");
    fflush(fptr);

    set_sigint_handler();
    init_clients_list();
    curr_id = 1;

    if ((server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, Q_PERMISSION, NULL)) == -1) {
        error("Error while creating server queue");
    }
    if (atexit(delete_queue) != 0) error("Cannot set exit function");
    printf("[server] server_queue = %d\n", server_queue);


    while (1) {
        if (mq_receive(server_queue, client_msg_str, MAX_MSG_SIZE, NULL) == -1) {
            error("Error while receiving message");
        }
        parse_client_msg_str();
        execute_client_order();
        sleep(1);
    }
}


void parse_client_msg_str() {
    char *token;

    token = strtok(client_msg_str, "_");
    client_msg.mtype = atol(token);

    token = strtok(NULL, "_");
    time_t t = atol(token);
    client_msg.mtext.time = *localtime(&t);

    token = strtok(NULL, "_");
    client_msg.mtext.id = atoi(token);

    token = strtok(NULL, "_");
    client_msg.mtext.info = atoi(token);

    token = strtok(NULL, "_");
    if (token == NULL) {
        strcpy(client_msg.mtext.text, "");
    }
    else {
        strcpy(client_msg.mtext.text, token);
    }

//    struct tm *tm = &client_msg.mtext.time;
//    printf("[%d-%02d-%02d %02d:%02d:%02d] TYPE: %ld, ID: %d, INFO: %d, TEXT: %s\n",
//           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
//           client_msg.mtype, client_msg.mtext.id, client_msg.mtext.info, client_msg.mtext.text);
}


void execute_client_order() {
    switch (client_msg.mtype) {
        case MSG_STOP:
            printf(">>> STOP\n");
            msg_stop();
            break;
        case MSG_LIST:
            printf(">>> LIST\n");
            msg_list();
            break;
        case MSG_INIT:
            printf(">>> INIT\n");
            msg_init();
            break;
        case MSG_2ALL:
            printf(">>> 2ALL\n");
            msg_2all();
            break;
        case MSG_2ONE:
            printf(">>> 2ONE\n");
            msg_2one();
            break;
        default:
            break;
    }
}


void msg_stop() {
    printf("[server] Client stopped, id: %d\n", client_msg.mtext.id);
    save_log("STOP", client_msg.mtext.id, &client_msg.mtext.time, NULL, NULL);
    int i = 0;
    while (i < MAX_NUM_OF_CLIENTS && clients[i].id != client_msg.mtext.id) ++i;
    if (i < MAX_NUM_OF_CLIENTS) {
        if (mq_close(clients[i].msqid) == -1) {
            error("Error while closing client queue");
        }
        clients[i].id = -1;
    }
}


void msg_list() {
    save_log("LIST", client_msg.mtext.id, &client_msg.mtext.time, NULL, NULL);
    printf("------ Active clients ------\n");
    printf("%-10s %-10s %-10s\n", "client_id", "mqd", "pid");
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        if (clients[i].id != -1) {
            printf("%-10d %-10d %-10d\n", clients[i].id, clients[i].msqid, clients[i].pid);
        }
    }
}


void msg_init() {
    save_log("INIT", curr_id, &client_msg.mtext.time, NULL, NULL);

    mqd_t client_queue;
    char client_queue_name[20];
    sprintf(client_queue_name, "/queue_%d", client_msg.mtext.id);

    if ((client_queue = mq_open(client_queue_name, O_WRONLY)) == -1) {
        error("Error while creating server queue");
    }

    int i = 0;
    while (i < MAX_NUM_OF_CLIENTS && clients[i].id != -1) ++i;
    if (i == MAX_NUM_OF_CLIENTS) {
        printf("[server] Maximum number of clients reached!\n");
        sprintf(server_msg_str, "%d_%ld_%d_%d_%s", MSG_INIT, (long)0, -1, -1, "");
        if (mq_send(client_queue, server_msg_str, strlen(server_msg_str) + 1, 1) == -1) {
            error("Error while sending INIT message");
        }

    } else {
        clients[i].id = curr_id;
        clients[i].msqid = client_queue;
        clients[i].pid = client_msg.mtext.id;

        sprintf(server_msg_str, "%d_%ld_%d_%d_%s", MSG_INIT, (long)0, curr_id, -1, "");
        if (mq_send(client_queue, server_msg_str, strlen(server_msg_str) + 1, 1) == -1) {
            error("Error while sending INIT message");
        }

        printf("[server] New client with id: %d\n", curr_id);

        ++curr_id;
    }
}


void msg_2all() {
    save_log("2ALL", client_msg.mtext.id, &client_msg.mtext.time, client_msg.mtext.text, NULL);

    time_t t = time(NULL);
    sprintf(server_msg_str, "%d_%ld_%d_%d_%s", MSG_2ALL, t, client_msg.mtext.id, 0, client_msg.mtext.text);
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        if (clients[i].id == -1) continue;
        if (mq_send(clients[i].msqid, server_msg_str, strlen(server_msg_str) + 1, 1) == -1) {
            error("Error while sending 2ALL message");
        }
    }
}


void msg_2one() {
    save_log("2ONE", client_msg.mtext.id, &client_msg.mtext.time, client_msg.mtext.text, &client_msg.mtext.info);

    int i = 0;
    while (i < MAX_NUM_OF_CLIENTS && clients[i].id != client_msg.mtext.info) ++i;
    if (i < MAX_NUM_OF_CLIENTS) {
        time_t t = time(NULL);
        sprintf(server_msg_str, "%d_%ld_%d_%d_%s", MSG_2ONE, t, client_msg.mtext.id, clients[i].id, client_msg.mtext.text);
        if (mq_send(clients[i].msqid, server_msg_str, strlen(server_msg_str) + 1, 1) == -1) {
            error("Error while sending 2ALL message");
        }
    }
}


void error(char *message) {
    if (errno != 0) {
        perror(message);
    }
    else {
        fprintf(stderr, "%s\n", message);
    }
    exit(EXIT_FAILURE);
}


void save_log(char *type, int id, const struct tm *tm, const char *msg, const int *receiver_id) {
    if (msg == NULL) {
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] MTYPE: %s, ID: %d\n",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, type, id);
    }
    else if (receiver_id == NULL) {
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] MTYPE: %s, ID: %d, MESSAGE: %s\n",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, type, id, msg);
    }
    else {
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] MTYPE: %s, ID: %d, RECEIVER_ID: %d, MESSAGE: %s\n",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, type, id, *receiver_id, msg);
    }
    fflush(fptr);
}


void delete_queue() {
    printf("\n[delete_queue]\n");
    if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
        fprintf(stderr, "%s\n", "Error while removing server queue");
    }
}


void close_log_file() {
    fclose(fptr);
}


void sigint_handler() {
    sprintf(server_msg_str, "%d_%ld_%d_%d_%s", MSG_STOP, (long)0, 0, 0, "");

    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        if (clients[i].id == -1) continue;

        if (mq_send(clients[i].msqid, server_msg_str, strlen(server_msg_str) + 1, 1) == -1) {
            error("Error while sending STOP message");
        }
        if (mq_receive(server_queue, client_msg_str, MAX_MSG_SIZE, NULL) == -1) {
            error("Error while receiving message");
        }
        parse_client_msg_str();
        printf("[server] Client stopped, id: %d\n", client_msg.mtext.id);
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


void init_clients_list() {
    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        clients[i].id = -1;
    }
}
