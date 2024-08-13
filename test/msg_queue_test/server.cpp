#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/msg.h>

typedef struct Msg{
    long msg_type;
    char msg_data[80];
}Msg;

#define SEND 100
#define RECV 200

int main(void){
    key_t msg_key = ftok("tmp", 7); 
    int msgid = msgget(msg_key, IPC_CREAT | 0755);
    printf("msgid: %d", msgid);

    if (msgid < 0)
    {
        return 0;
    }

    Msg msg;
    while(1){
        printf("Ser :>");
        scanf("%s", msg.msg_data);
        if(strncmp(msg.msg_data, "quit", 4) == 0){ 
            break;
        }   
        msg.msg_type = SEND;
        msgsnd(msgid, &msg, 80, 0);

        msg.msg_type = RECV;
        msgrcv(msgid, &msg, 80, RECV, 0);
        printf("Cli :>%s\n", msg.msg_data);
     }   
     return 0;
}