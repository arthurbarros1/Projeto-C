#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define PLAYER_START_X (MAXX / 2)
#define PLAYER_Y (MAXY - 2)
#define ALIEN_ROWS 3
#define ALIEN_COLS 10
#define MAX_BULLETS 5

typedef struct {
    int x, y;
    int active;
} Bullet;

typedef struct {
    int x, y;
    int alive;
} Alien;

int playerX;
Bullet bullets[MAX_BULLETS];
Alien aliens[ALIEN_ROWS][ALIEN_COLS];
int alienDirection = 1;  
int gameOver = 0;
int score = 0;  
void initializeGame();
void initializeAliens();
void drawGame();
void updateGame();
void handleInput();
void shootBullet();
void moveBullets();
void moveAliens();
int checkCollision(Bullet *bullet, Alien *alien);
void showGameOver();

int main() {
    keyboardInit();
    screenInit(1);  
    timerInit(100); 

    initializeGame();

    while (!gameOver) {
        if (keyhit()) {
            handleInput();
        }
        if (timerTimeOver()) {
            updateGame();
            drawGame();
        }
        usleep(5000); 
    }

    showGameOver();

    screenDestroy();
    keyboardDestroy();
    timerDestroy();
    return 0;
}

void initializeGame() {
    playerX = PLAYER_START_X;
    initializeAliens();  

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
}

void initializeAliens() {
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLS; j++) {
            
            aliens[i][j].x = SCRSTARTX + j * 3;
            aliens[i][j].y = SCRSTARTY + i * 2 + 1;  
            aliens[i][j].alive = 1;
        }
    }
    alienDirection = 1; 
}

void drawGame() {
    screenClear();
    screenDrawBorders();

   
    screenSetColor(BLUE, BLACK);
    screenGotoxy(playerX, PLAYER_Y);
    printf("A");

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            screenGotoxy(bullets[i].x, bullets[i].y);
            printf("|");
        }
    }

    screenSetColor(RED, BLACK);
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLS; j++) {
            if (aliens[i][j].alive) {
                screenGotoxy(aliens[i][j].x, aliens[i][j].y);
                printf("M");
            }
        }
    }

    screenSetColor(WHITE, BLACK);
    screenGotoxy(35, -2);  
    printf("Score: %d", score);

    screenUpdate();
}

void updateGame() {
    moveBullets();
    moveAliens();

    int allDead = 1;
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLS; j++) {
            if (aliens[i][j].alive) {
                allDead = 0;
                break;
            }
        }
        if (!allDead) break;
    }

    if (allDead) {
        initializeAliens();  
    }
}

void handleInput() {
    int ch = readch();

    if (ch == 'a' && playerX > SCRSTARTX) {
        playerX--;
    } else if (ch == 'd' && playerX < SCRENDX) {
        playerX++;
    } else if (ch == ' ') {
        shootBullet();
    } else if (ch == 'q') {
        screenDestroy();
        keyboardDestroy();
        timerDestroy();
        exit(0);
    }
}

void shootBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = playerX;
            bullets[i].y = PLAYER_Y - 1;
            bullets[i].active = 1;
            break;
        }
    }
}

void moveBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y--;

            if (bullets[i].y < SCRSTARTY) {
                bullets[i].active = 0;
            } else {
                for (int j = 0; j < ALIEN_ROWS; j++) {
                    for (int k = 0; k < ALIEN_COLS; k++) {
                        if (aliens[j][k].alive && checkCollision(&bullets[i], &aliens[j][k])) {
                            aliens[j][k].alive = 0;
                            bullets[i].active = 0;
                            score += 2;  
                            break;
                        }
                    }
                }
            }
        }
    }
}

void moveAliens() {
    int reachedEdge = 0;

    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLS; j++) {
            if (aliens[i][j].alive) {
                aliens[i][j].x += alienDirection;

                if (aliens[i][j].x <= SCRSTARTX || aliens[i][j].x >= SCRENDX) {
                    reachedEdge = 1;
                }
            }
        }
    }

    if (reachedEdge) {
        alienDirection *= -1;
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLS; j++) {
                if (aliens[i][j].alive) {
                    aliens[i][j].y += 1;  
                    if (aliens[i][j].y >= PLAYER_Y) {
                        gameOver = 1;  
                    }
                }
            }
        }
    }
}

int checkCollision(Bullet *bullet, Alien *alien) {
    return bullet->x == alien->x && bullet->y == alien->y;
}

void showGameOver() {
    screenClear();
    
    screenDrawBorders();

    screenSetColor(RED, BLACK);

    int centerX = (MAXX - 8) / 2;  
    int centerY = MAXY / 2;

    screenGotoxy(centerX, centerY);
    printf("GAME OVER");
    fflush(stdout);  

    screenGotoxy(centerX - 5, centerY + 1); 
    printf("Final Score--> %d", score);
    fflush(stdout);

    screenSetColor(GREEN, BLACK);
    screenGotoxy((MAXX - 18) / 2, centerY + 2);  
    
    usleep(2000000);  
    
    printf("Press 'q' to quit.");
    fflush(stdout);

    while (1) {
        if (keyhit() && readch() == 'q') {
            break;
        }
    }
}
