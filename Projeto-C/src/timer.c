#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include "timer.h"

static struct timespec start;
static int delayMs;

// Inicializa o temporizador com o delay em milissegundos
void timerInit(int delay) {
    delayMs = delay;
    clock_gettime(CLOCK_REALTIME, &start);
}

// Verifica se o tempo de delay se passou desde o último reset
int timerTimeOver() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    
    long diff = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
    
    if (diff >= delayMs) {
        start = now; // Reseta o tempo
        return 1;
    }
    return 0;
}

// Destroi o temporizador (não é necessário liberar nada)
void timerDestroy() {
    // Sem operação para destruir
}
