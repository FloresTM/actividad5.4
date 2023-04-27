// Actividad 5.4 Problemas de sincronización
// David Langarica Hernandez | A01708936
// Sebastian Flores Lemus | A01709229
#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <cmath>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

const int NUM_POINTS = 1000000;
const int NUM_THREADS = 8;

// Variable global para almacenar el número de puntos dentro del círculo.
int points_in_circle = 0;
pthread_mutex_t mutex_points = PTHREAD_MUTEX_INITIALIZER;

void* monte_carlo(void *param) {
    int local_points_in_circle = 0;
    unsigned int seed = time(NULL);

    // Generador de números aleatorios.
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < NUM_POINTS / NUM_THREADS; ++i) {
        double x = distribution(generator);
        double y = distribution(generator);

        if (x * x + y * y <= 1.0) {
            local_points_in_circle++;
        }
    }

    // Bloqueo del mutex y actualización de la variable global.
    pthread_mutex_lock(&mutex_points);
    points_in_circle += local_points_in_circle;
    pthread_mutex_unlock(&mutex_points);

    pthread_exit(0);
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, monte_carlo, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi_estimate = 4.0 * points_in_circle / NUM_POINTS;
    cout << "Aproximación de Pi: " << setprecision(15) << pi_estimate << endl;

    pthread_mutex_destroy(&mutex_points);

    return 0;
}
