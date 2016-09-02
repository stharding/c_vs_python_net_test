/* adapted from https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/udpserver.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define ROWS 1000

void error(char *msg) {
    perror(msg);
    exit(1);
}

int compare( const void* a, const void* b)
{
     double int_a = *((double*)a);
     double int_b = *((double*)b);

     if(int_a == int_b) return 0;
     else if (int_a < int_b) return -1;
     else return 1;
}

int main(int argc, char **argv) {
    int sockfd; /* socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    char* fn;
    int buffsize = 60000;
    char buf[buffsize]; /* message buf */
    int max_rows = ROWS;
    char* rows[ROWS];
    double diffs[ROWS];
    struct timeval tv;
    int line_num = 0;
    int i;
    char output_file_name[ROWS];
    FILE* output_file = NULL;

    for(i = 0; i < ROWS; ++i) {
        rows[i] = malloc(20000);
        bzero(rows[i], 20000);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    portno = 31337;
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        error("ERROR on binding");
    }

    clientlen = sizeof(clientaddr);
    while (1) {

        bzero(buf, buffsize);
        n = recvfrom(sockfd, buf, buffsize, 0, (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0) error("ERROR in recvfrom");

        char* pkt_time_str = strtok(buf, "!");

        if(strcmp("done", pkt_time_str) ==  0) {
            fn = strtok(NULL, "!");
            sprintf(output_file_name, "network_test_c_%s", fn);
            output_file = fopen(output_file_name, "w");

            break;
        }

        double pkt_time = atof(pkt_time_str);
        char* rest = (char*)buf + strlen(pkt_time_str) + 1;
        char upper_rest[20000];
        bzero(upper_rest, 20000);
        int rest_len = strlen(rest);
        for(i = 0; i < rest_len; ++i) {
            upper_rest[i] = toupper(rest[i]);
        }

        gettimeofday(&tv, NULL);
        char current_time_str[ROWS];
        sprintf(current_time_str, "%ld.%d", tv.tv_sec, tv.tv_usec);
        double current_time =  atof(current_time_str);
        double difference = current_time - pkt_time;
        double diff = current_time - pkt_time;
        sprintf(rows[line_num], "%0.30lf\t%0.30lf\t%0.30lf\t%s", current_time, pkt_time, diff, upper_rest);
        diffs[line_num] = diff;
        ++line_num;
    }

    if(output_file) {
        for(i = 0; i < ROWS; ++i) {
            fprintf(output_file, "%s\n", rows[i]);
            free(rows[i]);
        }
        double average = 0;
        qsort(diffs, ROWS, sizeof(double), compare);
        for(i = 0; i < ROWS; ++i) {
            average += diffs[i];
        }
        average /= ROWS;
        double median = diffs[50];
        fprintf(output_file, "\naverage difference: %lf\n", average);
        fprintf(output_file, "median difference: %lf\n", median);
        printf("\naverage difference: %lf\n", average);
        printf("median difference: %lf\n", median);
        fclose(output_file);
    }
}
