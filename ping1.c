#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


volatile sig_atomic_t finish = 0;
volatile sig_atomic_t end = 0;
volatile sig_atomic_t received_value;


void handler_rts(int signum, siginfo_t *info, void *ucontext) {
    sleep(1);
    received_value = info->si_value.sival_int;
    finish = 1;
}

void handler_term() {
    sleep(1);
    end = 1;
    finish = 1;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 0;
    }

    int pong_pid = atoi(argv[1]);
    printf("My PID is: %d\n", getpid());

    int value;
    printf("Enter a value: ");
    scanf("%d", &value);


    struct sigaction action_term;
    memset(&action_term, 0, sizeof(action_term));
    action_term.sa_handler = handler_term;
    action_term.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_term, NULL);

    struct sigaction action_rts;
    memset(&action_rts, 0, sizeof(action_rts));
    action_rts.sa_sigaction = handler_rts;
    action_rts.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGRTMIN, &action_rts, NULL);

    while(!end) {
        printf("ping %d\n", value);
        fflush(stdout);


        union sigval value_action_rts;
        value_action_rts.sival_int = value;
        sigqueue(pong_pid, SIGRTMIN, value_action_rts);

        //while (!finish) {
         //   sleep(1);
       // }
        printf("Pong received, with value %d\n", received_value);
        fflush(stdout);

        value = received_value;
        finish = 0;
    }
    printf("Ping received SIGTERM signal. Good-bye!!\n");
    fflush(stdout);

    int signal_sent = kill(pong_pid, SIGTERM);
    if (signal_sent == -1) {
        printf("Kill failed\n");
        return -1;
    }
}


