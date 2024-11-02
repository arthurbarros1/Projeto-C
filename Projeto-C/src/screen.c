#include <stdio.h>
#include "screen.h"

// Inicializa a tela e ativa o uso de cores, se suportado
void screenInit(int enableColor) {
    if (enableColor) {
        printf("\033[0m"); // Reset color
    }
    screenClear();
}

// Define as cores de texto e fundo
void screenSetColor(int fg, int bg) {
    printf("\033[%d;%dm", fg, bg + 10);
}

// Posiciona o cursor na posição (x, y)
void screenGotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Limpa a tela
void screenClear() {
    printf("\033[2J");
    screenGotoxy(0, 0);
}

// Atualiza a tela (no nosso caso, apenas força um refresh)
void screenUpdate() {
    fflush(stdout);
}

// Restaura as configurações da tela e reinicializa a cor
void screenDestroy() {
    printf("\033[0m");
}
