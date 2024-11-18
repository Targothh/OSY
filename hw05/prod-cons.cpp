#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdio>
#include <list>
#include <semaphore.h>

using namespace std;

struct command_t {
    int num;
    string name;
};

list<command_t*> queue;
pthread_mutex_t queue_mutex;
pthread_mutex_t stdout_mutex;
sem_t commands_rem;
bool no_commands = false;
int return_val = 0;

void *producent(void *arg){
    int ret, x;
    char *text;
    while ((ret = scanf("%d %ms", &x, &text)) == 2) {
        if (x < 0) {
            exit (1);
        }
        command_t *command = new command_t{x, text};
        free(text);

        pthread_mutex_lock(&queue_mutex);
        queue.push_back(command);
        pthread_mutex_unlock(&queue_mutex);

        sem_post(&commands_rem);
    }
    if (ret == 0){
        return_val = 1;
    }
    no_commands = true;
    for (int i = 0; i < *(int*)arg; i++){
        sem_post(&commands_rem);
    }
    return nullptr;
}

void *consument(void *arg){
    int my_id = *(int*)arg;
    delete (int*)arg;
    while (true) {
        sem_wait(&commands_rem);
        pthread_mutex_lock(&queue_mutex);
        if (queue.empty() && no_commands) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }
        command_t *command = queue.front();
        queue.pop_front();
        pthread_mutex_unlock(&queue_mutex);
        pthread_mutex_lock(&stdout_mutex);
        cout << "Thread " << my_id << ":";
        for (int i = 0; i < command->num; i++){
            cout << " " << command->name;
        }
        cout << endl;
        pthread_mutex_unlock(&stdout_mutex);
        delete command;
    }
    return nullptr;
}

int main(int argc, char **argv){
    int cons_count;

    if (argc == 2) {
        cons_count = stoi(argv[1]);
        if (sysconf(_SC_NPROCESSORS_ONLN) < cons_count){
            exit(1);
        }
    } else if (argc > 2){
        exit(1);
    } else {
        cons_count = 1;
    }
    sem_init(&commands_rem, 0, 0);
    pthread_t prod_th;
    pthread_create(&prod_th, nullptr, producent, &cons_count);
    vector<pthread_t> cons_th(cons_count);
    for (int i = 0; i < cons_count; i++){
        int *cons_id = new int(i + 1);
        pthread_create(&cons_th[i], nullptr, consument, cons_id);
    }
    pthread_join(prod_th, nullptr);
    for (auto thread : cons_th) {
        pthread_join(thread, nullptr);
    }
    sem_destroy(&commands_rem);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&stdout_mutex);
    return return_val;
}