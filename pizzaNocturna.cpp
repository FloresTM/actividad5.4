// Actividad 5.4 Problemas de sincronización
// David Langarica Hernandez | A01708936
// Sebastian Flores Lemus | A01709229
#include <pthread.h>
#include <unistd.h>

#include <iostream>

using namespace std;

const int NUM_ESTUDIANTES = 5;
const int S = 8;

int rebanadas = S;
bool pizza_agotada = false;
pthread_mutex_t mutex_pizza = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_estudiantes = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_pizzero = PTHREAD_COND_INITIALIZER;

void *estudiante(void *param) {
    int id = *((int *)param);
    while (true) {
        pthread_mutex_lock(&mutex_pizza);

        while (rebanadas == 0) {
            if (!pizza_agotada) {
                pizza_agotada = true;
                pthread_cond_signal(&cv_pizzero);
            }
            pthread_cond_wait(&cv_estudiantes, &mutex_pizza);
        }

        rebanadas--;
        cout << "Estudiante " << id << " comiendo una rebanada. Quedan " << rebanadas << " rebanadas." << endl;

        pthread_mutex_unlock(&mutex_pizza);
        sleep(rand() % 5 + 1);
    }
}

void *pizzero(void *param) {
    while (true) {
        pthread_mutex_lock(&mutex_pizza);

        while (!pizza_agotada) {
            pthread_cond_wait(&cv_pizzero, &mutex_pizza);
        }

        rebanadas = S;
        pizza_agotada = false;
        cout << "El pizzero entrega una nueva pizza. Hay " << rebanadas << " rebanadas." << endl;
        pthread_cond_broadcast(&cv_estudiantes);

        pthread_mutex_unlock(&mutex_pizza);
    }
}

int main() {
    pthread_t estudiantes[NUM_ESTUDIANTES];
    pthread_t pizzero_thread;
    int ids[NUM_ESTUDIANTES];

    pthread_create(&pizzero_thread, NULL, pizzero, NULL);

    for (int i = 0; i < NUM_ESTUDIANTES; i++) {
        ids[i] = i + 1;
        pthread_create(&estudiantes[i], NULL, estudiante, (void *)&ids[i]);
    }

    for (int i = 0; i < NUM_ESTUDIANTES; i++) {
        pthread_join(estudiantes[i], NULL);
    }

    pthread_join(pizzero_thread, NULL);

    pthread_mutex_destroy(&mutex_pizza);
    pthread_cond_destroy(&cv_estudiantes);
    pthread_cond_destroy(&cv_pizzero);

    return 0;
}