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

void process_ack(unsigned char *message){
    if (message[0] == 0x4F){
        printf("ACK received for heartrate %d BPM\n", message[2]);
    }
}

void hrm_broadcast(){
    int sock;
    struct sockaddr_in slave_addr, recv_addr; // from inet.h. fields: sin_family, sin_port, sin_addr, sin_zero
    unsigned char send_message[8];
    unsigned char recv_ack_message[8];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failure");
        exit(EXIT_FAILURE);
    }

    memset(&slave_addr, 0, sizeof(slave_addr)); // 0 the struct to a known state
    slave_addr.sin_family = AF_INET;
    slave_addr.sin_port = htons(5001); // convert to network byte order
    slave_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(5000); // listen on different port
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0){
        perror("ack listener bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Broadcasting HRM data...\n");

    srand(time(NULL)); // ensurre a new random seed is generated

    while(1){
        int heart_rate = rand() % 151 + 50; // random number between 50 and 200
        create_ant_message(send_message, heart_rate);

        if (sendto(sock, send_message, sizeof(send_message), 0, (const struct sockaddr *)&slave_addr, sizeof(slave_addr)) < 0){
            perror("sendto failed");
        }
        else{
            printf("HRM sent Heart rate: %d\n", heart_rate);
        }

        if (recvfrom(sock, recv_ack_message, sizeof(recv_ack_message), 0, NULL, NULL) < 0){
            perror("recvfrom failed");
        }
        else{
            process_ack(recv_ack_message);
        }

        usleep(250000); // 250ms: 4Hz
    }

    close(sock);
}

int main(){
    hrm_broadcast();
    return 0;
}