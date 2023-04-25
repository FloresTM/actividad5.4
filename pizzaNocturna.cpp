// Actividad 5.4 Problemas de sincronización
// David Langarica Hernandez | A01708936
// Sebastian Flores Lemus | A01709229

#include <pthread.h>
#include <unistd.h>

#include <iostream>

using namespace std;

// número de hilos de estudiante
const int NUM_ESTUDIANTES = 5;

// número de rebanadas por pizza
const int S = 8;

int rebanadas = S;
bool pizza_agotada = false;

// Se crean los mutex y las condiciones para sincronizar los hilos
pthread_mutex_t mutex_pizza = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_estudiantes = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_pizzeria = PTHREAD_COND_INITIALIZER;

// Función que ejecuta cada estudiante
void *estudiante(void *param) {
    int id = *((int *)param);
    while (true) {
        pthread_mutex_lock(&mutex_pizza);

        while (rebanadas == 0) {

            // Si se ha agotado la pizza, se pide una nueva
            if (!pizza_agotada) {
                pizza_agotada = true;
                pthread_cond_signal(&cv_pizzeria);
            }

            // El estudiante espera a que se entregue una nueva pizza
            pthread_cond_wait(&cv_estudiantes, &mutex_pizza);
        }

        // El estudiante toma una rebanada de pizza y disminuye el número de rebanadas disponibles
        rebanadas--;
        cout << "Estudiante " << id << " comiendo una rebanada. Quedan " << rebanadas << " rebanadas." << endl;

        // Libera el mutex para permitir que otros estudiantes accedan a la pizza
        pthread_mutex_unlock(&mutex_pizza);
        sleep(rand() % 5 + 1);
    }
}

void *pizzeria(void *param) {
    while (true) {
        pthread_mutex_lock(&mutex_pizza);

        // Mientras no se haya agotado la pizza, la pizzeria espera
        while (!pizza_agotada) {
            pthread_cond_wait(&cv_pizzeria, &mutex_pizza);
        }

        // Si la pizza se agotó, la pizzeria prepara una nueva pizza
        rebanadas = S;
        pizza_agotada = false;
        cout << "La pizzeria entrega una nueva pizza. Hay " << rebanadas << " rebanadas." << endl;
        pthread_cond_broadcast(&cv_estudiantes);

        // Libera el mutex para permitir que los estudiantes accedan a la pizza
        pthread_mutex_unlock(&mutex_pizza);
    }
}

int main() {
    pthread_t estudiantes[NUM_ESTUDIANTES];
    pthread_t pizzeria_thread;
    int ids[NUM_ESTUDIANTES];

    // Se crea el hilo de la pizzeria
    pthread_create(&pizzeria_thread, NULL, pizzeria, NULL);

    // Se crean los hilos de los estudiantes
    for (int i = 0; i < NUM_ESTUDIANTES; i++) {
        ids[i] = i + 1;
        pthread_create(&estudiantes[i], NULL, estudiante, (void *)&ids[i]);
    }

    // Se espera a que todos los hilos de los estudiantes terminen de ejecutarse
    for (int i = 0; i < NUM_ESTUDIANTES; i++) {
        pthread_join(estudiantes[i], NULL);
    }

    // Se espera a que el hilo de la pizzeria termine de ejecutarse
    pthread_join(pizzeria_thread, NULL);

    // Se destruyen los mutexes y las variables de condición
    pthread_mutex_destroy(&mutex_pizza);
    pthread_cond_destroy(&cv_estudiantes);
    pthread_cond_destroy(&cv_pizzeria);

    return 0;
}