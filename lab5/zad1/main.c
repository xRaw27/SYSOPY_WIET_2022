//
// Created by xraw on 19.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_CMD_COMPONENTS 100
#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

char **parse(char *cmd) {
    static char *parsed[MAX_ARGS];

    int i = 0;
    char *end_str;
    char *arg = strtok_r(cmd, " ", &end_str);

    while (arg != NULL) {
        parsed[i] = calloc(strlen(arg) + 1, sizeof(char));
        strcpy(parsed[i++], arg);
        arg = strtok_r(NULL, " ", &end_str);
    }
    while (i < MAX_ARGS) {
        parsed[i++] = NULL;
    }

    return parsed;
}


void execute(char **commands, int i, char *cmd) {
    printf("\n *** EXECUTING COMMAND: ");

    char buffer[MAX_CMD_LEN] = "";
    char *component = strtok(cmd, "|");
    while (component != NULL) {
        while (!isdigit(component[0])) ++component;
        int component_id = atoi(component) - 1;
        if (component_id < 0 || component_id >= i) error("Wrong component number");
        strcat(buffer, commands[component_id]);
        strcat(buffer, " | ");
        component = strtok(NULL, "|");
    }

    printf("%s\n", buffer);

    int current_fd[2];
    int prev_fd[2];
    pipe(prev_fd);
    close(prev_fd[0]);
    close(prev_fd[1]);

    char *end_str;
    component = strtok_r(buffer, "|", &end_str);
    while (component != NULL && strlen(component) > 1) {
//        printf("%s ->", component);
        pipe(current_fd);
        if (fork() == 0) {
            dup2(prev_fd[0], STDIN_FILENO);
            dup2(current_fd[1], STDOUT_FILENO);
            close(prev_fd[1]);
            close(current_fd[0]);

            char **parsed = parse(component);
            execvp(parsed[0], parsed);
        }
        wait(NULL);

        close(current_fd[1]);
        prev_fd[0] = current_fd[0];
        prev_fd[1] = current_fd[1];
        component = strtok_r(NULL, "|", &end_str);
    }

    char c;
    while (read(current_fd[0], &c, 1) == 1) {
        printf("%c", c);
    }
    close(current_fd[0]);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        error("One argument expected");
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        error("Error while opening input file");
    }

    char **commands = calloc(MAX_CMD_COMPONENTS, sizeof(char*));

    char *line = NULL;
    size_t len = 0, n;
    int i = 0, mode = 0;

    while ((n = getline(&line, &len, in)) != -1) {
        if (mode == 0) {
            int j = 0, k;
            while (j < n && line[j] != '=') ++j;
            while (j < n && (line[j] == '=' || line[j] == ' ')) ++j;

            if (j == n) {
                mode = 1;
                continue;
            }

            commands[i] = calloc(n - j + 1, sizeof(char));
            for (k = j; k < n; ++k) {
                if (line[k] == '\n') break;
                commands[i][k - j] = line[k];
            }
            commands[i++][k - j] = '\0';
        }
        else {
            execute(commands, i, line);
        }
    }

    free(line);
    fclose(in);
    for (int j = 0; j < i; ++j) {
        free(commands[j]);
    }
    free(commands);

    return 0;
}