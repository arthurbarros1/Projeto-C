#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define MAX_INVADERS 5
#define PLAYER_X_START 34
#define PLAYER_Y 23

typedef struct {
    int x, y;
    int alive;
} Invader;

typedef struct {
    int x, y;
    int active;
} Bullet;

Invader invaders[MAX_INVADERS];
Bullet bullet;
int playerX = PLAYER_X_START;
int gameRunning = 1;

void initGame() {
    
    for (int i = 0; i < MAX_INVADERS; i++) {
        invaders[i].x = 10 + i * 6;
        invaders[i].y = 5;
        invaders[i].alive = 1;
    }

    
    playerX = PLAYER_X_START;

    
    bullet.active = 0;
}

void drawInvaders() {
    screenSetColor(GREEN, DARKGRAY);
    for (int i = 0; i < MAX_INVADERS; i++) {
        if (invaders[i].alive) {
            screenGotoxy(invaders[i].x, invaders[i].y);
            printf("X");
        }
    }
}

void drawPlayer() {
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(playerX, PLAYER_Y);
    printf("A"); 
}

void drawBullet() {
    if (bullet.active) {
        screenSetColor(RED, DARKGRAY);
        screenGotoxy(bullet.x, bullet.y);
        printf("|");
    }
}

void moveInvaders() {
    for (int i = 0; i < MAX_INVADERS; i++) {
        if (invaders[i].alive) {
            invaders[i].y += 1;
            if (invaders[i].y >= PLAYER_Y) {
                gameRunning = 0; 
            }
        }
    }
}

void moveBullet() {
    if (bullet.active) {
        bullet.y -= 1;
        if (bullet.y < 1) bullet.active = 0;

        
        for (int i = 0; i < MAX_INVADERS; i++) {
            if (invaders[i].alive && bullet.active && bullet.x == invaders[i].x && bullet.y == invaders[i].y) {
                invaders[i].alive = 0;
                bullet.active = 0;
            }
        }
    }
}

void shootBullet() {
    if (!bullet.active) {
        bullet.x = playerX;
        bullet.y = PLAYER_Y - 1;
        bullet.active = 1;
    }
}

void handleInput(int ch) {
    if (ch == 'a' && playerX > 1) playerX -= 1;
    if (ch == 'd' && playerX < MAXX - 1) playerX += 1;
    if (ch == 'w') shootBullet();
}

int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();
    timerInit(50);

    initGame();

    while (gameRunning) {
        if (keyhit()) {
            ch = readch();
            handleInput(ch);
        }

        if (timerTimeOver() == 1) {
            moveInvaders();
            moveBullet();

            screenClear();
            drawInvaders();
            drawPlayer();
            drawBullet();
            screenUpdate();

            
            int allDead = 1;
            for (int i = 0; i < MAX_INVADERS; i++) {
                if (invaders[i].alive) {
                    allDead = 0;
                    break;
                }
            }
            if (allDead) {
                gameRunning = 0;
            }
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    if (gameRunning == 0) {
        printf("Game Over!\n");
    } else {
        printf("You Win!\n");
    }

    return 0;
}