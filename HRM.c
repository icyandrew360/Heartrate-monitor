#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>


void create_ant_message(unsigned char *message, int heart_rate){
    message[0] = 0x4E; // page number
    message[1] = 0x00;
    message[2] = heart_rate;
    for (int i = 3; i < 8; i++){
        message[i] = 0x00; // reserved
    }
}

void hrm_broadcast(){
    int sock;
    struct sockaddr_in slave_addr; // from inet.h. fields: sin_family, sin_port, sin_addr, sin_zero
    unsigned char message[8];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failure");
        exit(EXIT_FAILURE);
    }

    memset(&slave_addr, 0, sizeof(slave_addr)); // 0 the struct to a known state
    slave_addr.sin_family = AF_INET;
    slave_addr.sin_port = htons(5001); // convert to network byte order
    slave_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Broadcasting HRM data...\n");

    srand(time(NULL)); // ensurre a new random seed is generated

    while(1){
        int heart_rate = rand() % 151 + 50; // random number between 50 and 200
        create_ant_message(message, heart_rate);

        if (sendto(sock, message, sizeof(message), 0, (const struct sockaddr *)&slave_addr, sizeof(slave_addr)) < 0){
            perror("sendto failed");
        }
        else{
            printf("HRM sent Heart rate: %d\n", heart_rate);
        }

        usleep(250000); // 250ms: 4Hz
    }

    close(sock);
}

int main(){
    hrm_broadcast();
    return 0;
}