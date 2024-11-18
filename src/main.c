#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define PLAYER_START_X (MAXX / 2)
#define PLAYER_Y (MAXY - 2)
#define ALIEN_ROWS 3
#define ALIEN_COLS 10
#define MAX_BULLETS 5
#define MAX_NAME_LENGTH 20

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} Player;

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
char playerName[MAX_NAME_LENGTH];

void showMenu();
void showRanking();
void sortRanking();
void saveScore(const char *name, int score);
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

    while (1) {
        showMenu();
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
    }

    screenDestroy();
    keyboardDestroy();
    timerDestroy();
    return 0;
}

void showMenu() {
    int choice;
    while (1) {
        screenClear();
        screenDrawBorders();
        screenSetColor(WHITE, BLACK);

        int centerX = MAXX / 2;  
        int centerY = MAXY / 2;  

        screenGotoxy(centerX - 10, centerY - 3);
        printf("=== SPACE INVADERS ===");

        screenGotoxy(centerX - 10, centerY - 1);
        printf("1. Start Game");
        screenGotoxy(centerX - 10, centerY);
        printf("2. View Ranking");
        screenGotoxy(centerX - 10, centerY + 1);
        printf("3. Exit");

        screenGotoxy(centerX - 10, centerY + 3);
        printf("Enter your choice: ");

        choice = getchar();
        while (getchar() != '\n');

        switch (choice) {
            case '1':
                screenClear();
                screenGotoxy(centerX - 10, centerY);
                printf("Enter your name: ");
                fgets(playerName, MAX_NAME_LENGTH, stdin);
                playerName[strcspn(playerName, "\n")] = '\0';  
                return;
            case '2':
                showRanking();
                break;
            case '3':
                screenDestroy();
                keyboardDestroy();
                timerDestroy();
                exit(0);
            default:
                screenGotoxy(centerX - 10, centerY + 5);
                printf("Invalid choice. Press any key to try again...");
                getchar();
        }
    }
}

void showRanking() {
    FILE *file = fopen("ranking.txt", "r");
    if (!file) {
        file = fopen("ranking.txt", "w");  
        fclose(file);
        file = fopen("ranking.txt", "r");
    }

    screenClear();
    screenDrawBorders();

    screenSetColor(YELLOW, BLACK);
    screenGotoxy(10, 5);
    printf("=== RANKING ===");

    screenSetColor(WHITE, BLACK);
    int rank = 1;
    char name[MAX_NAME_LENGTH];
    int score;
    while (fscanf(file, "%s %d", name, &score) != EOF) {
        screenGotoxy(10, 6 + rank);
        printf("%d. %s - %d points", rank, name, score);
        rank++;
    }

    fclose(file);

    screenGotoxy(10, 20);
    printf("Press any key to return to menu...");
    getchar();
}

void saveScore(const char *name, int score) {
    FILE *file = fopen("ranking.txt", "a");
    if (!file) {
        printf("Error saving score!\n");
        return;
    }
    fprintf(file, "%s %d\n", name, score);
    fclose(file);

    sortRanking();
}

void sortRanking() {
    FILE *file = fopen("ranking.txt", "r");
    Player players[100];
    int count = 0;

    while (fscanf(file, "%s %d", players[count].name, &players[count].score) != EOF) {
        count++;
    }
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (players[j].score > players[i].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    file = fopen("ranking.txt", "w");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d\n", players[i].name, players[i].score);
    }
    fclose(file);
}

void initializeGame() {
    playerX = PLAYER_START_X;
    initializeAliens();

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    score = 0;
    gameOver = 0;
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
                    aliens[i][j].y++;
                }
            }
        }
    }

    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLS; j++) {
            if (aliens[i][j].alive && aliens[i][j].y >= PLAYER_Y) {
                gameOver = 1;
                return;
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

    screenGotoxy(centerX - 5, centerY + 1);
    printf("Final Score --> %d", score);

    saveScore(playerName, score);

    screenGotoxy((MAXX - 18) / 2, centerY + 2);
    printf("Returning to menu...");
    usleep(2000000);
}
