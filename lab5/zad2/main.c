//
// Created by xraw on 20.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


void read_mails(const char *mode) {
    FILE *mail_output = NULL;

    if (strcasecmp(mode, "nadawca") == 0) {
        mail_output = popen("mail -H | sort -k 3", "r");
    }
    else if (strcasecmp(mode, "data") == 0) {
        mail_output = popen("mail -H", "r");
    }
    else {
        error("Wrong read mode argument");
    }

    char buffer[256];

    while (fgets(buffer, 256, mail_output) != NULL) {
        printf("%s", buffer);
    }

    if (pclose(mail_output) == -1) {
        error("pclose error");
    }
}

void send_mail(const char *to, const char *subject, const char *message) {
    size_t len = strlen(to) + strlen(subject);
    char *buffer = calloc(len + 20, sizeof(char));
    sprintf(buffer, "mail -s \"%s\" %s", subject, to);

    FILE *mail_input = popen(buffer, "w");
    fputs(message, mail_input);

    free(buffer);
    if (pclose(mail_input) == -1) {
        error("pclose error");
    }
    printf("Mail \"%s\" sent to %s\n", subject, to);
}


int main(int argc, char **argv) {
    switch (argc)
    {
        case 2:
            read_mails(argv[1]);
            break;

        case 4:
            send_mail(argv[1], argv[2], argv[3]);
            break;
        default:
            error("Wrong number of arguments");
    }

    return 0;
}