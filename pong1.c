#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


volatile sig_atomic_t finish = 0;
volatile sig_atomic_t end = 0;
volatile sig_atomic_t received_value;
volatile sig_atomic_t ping_pid;

void handler_rts(int signum, siginfo_t *info, void *ucontext) {
    sleep(1);
    received_value = info->si_value.sival_int;
    ping_pid = info->si_pid;
    finish = 1;
}

void handler_term() {
    sleep(1);
    end = 1;
    finish = 1;
}


int main() {
    printf("My PID is: %d\n", getpid());

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

        //while (!finish) {
            //sleep(1);
        //}
        printf("Ping received, with value %d from %d\n", received_value, ping_pid);
        fflush(stdout);
        finish = 0;

        received_value += 1;
        union sigval value_action_rts;
        value_action_rts.sival_int = received_value;
        sigqueue(ping_pid, SIGRTMIN, value_action_rts);

        printf("pong %d\n", received_value);
        fflush(stdout);
    }
    printf("Pong received SIGTERM signal. Good-bye!!\n");
    fflush(stdout);

}


