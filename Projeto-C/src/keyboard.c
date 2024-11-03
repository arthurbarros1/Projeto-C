#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "keyboard.h"
#include <sys/select.h>


static struct termios oldt, newt;

// Configura o terminal para o modo sem buffer
void keyboardInit() {
    tcgetattr(STDIN_FILENO, &oldt); // Salva o estado atual do terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Desabilita o modo canônico e o eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica as configurações
}

// Verifica se uma tecla foi pressionada
int keyhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

// Lê a tecla pressionada
int readch() {
    int ch;
    ch = getchar();
    return ch;
}

// Restaura as configurações originais do terminal
void keyboardDestroy() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
