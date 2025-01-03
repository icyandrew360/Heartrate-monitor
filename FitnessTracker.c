#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_HEART_RATE_VALUES 100

void extract_heartrate_from_message(unsigned char *message, int *heart_rate){
    *heart_rate = message[2];
}

void create_ack_message(unsigned char *message, int *heart_rate){
    message[0] = 0x4F;
    message[1] = 0x00; 
    message[2] = *heart_rate;
    for (int i = 3; i < 8; i++){
        message[i] = 0x00;
    }
}

void fitness_tracker(){
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in ack_addr;
    unsigned char message[8];
    unsigned char ack_message[8];
    int heart_rates[MAX_HEART_RATE_VALUES];
    int count = 0;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failure");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(&ack_addr, 0, sizeof(ack_addr));
    ack_addr.sin_family = AF_INET;
    ack_addr.sin_port = htons(5000);
    ack_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Fitness tracker running...\n");

    while(1){
        if (recvfrom(sock, message, sizeof(message), 0, NULL, NULL) < 0){
            perror("recv failed");
            continue;
        }
        else{
            int heart_rate = 0;
            extract_heartrate_from_message(message, &heart_rate);
            heart_rates[count] = heart_rate;
            count++;

            printf("recvd heart rate: %d\n", heart_rate);

            create_ack_message(ack_message, &heart_rate);

            if (sendto(sock, ack_message, sizeof(ack_message), 0, (const struct sockaddr*) &ack_addr, sizeof(ack_addr)) < 0){
                perror("sendto failed");
            }
            else{
                printf("ACK sent for heart rate: %d\n", heart_rate);
            }
        }

        // generate metrics every for the last 10 messages
        if (count % 10 == 0 && count != 0){
            int sum = 0;
            int max = 0;
            int min = 200;
            for (int i = count - 10; i < count; i++){
                if (heart_rates[i] > max){
                    max = heart_rates[i];
                }
                if (heart_rates[i] < min){
                    min = heart_rates[i];
                }
                sum += heart_rates[i];
            }
            int avg = sum / 10;
            printf("Average heart rate for the last 10 messages: %d\n", avg);
            printf("Max heart rate for the last 10 messages: %d\n", max);
            printf("Min heart rate for the last 10 messages: %d\n", min);
        }

        if (count == MAX_HEART_RATE_VALUES){
            count = 0;
        }
    }
}

int main(){
    fitness_tracker();
    return 0;
}