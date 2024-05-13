#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define HEIGHT 23
#define WIDTH 80
#define BALLSPEED 50

int p1PosY, p2PosY, ballPosX, ballPosY;
int p1Score = 0, p2Score = 0;
int ballDirection = 0;
char p1KeyPressed, p2KeyPressed;

void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}

void quit() {
    //kill user control thread
    pthread_cancel(pthread_self());
    //show cursor
    printf("\033[?25h");
    exit(0);
}

void drawScreen() {
    system("clear");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void drawP1() {
    //draw the first paddle
    printf("\033[%d;%df", p1PosY - 2, 2);
    puts("|");
    printf("\033[%d;%df", p1PosY - 1, 2);
    puts("|");
    printf("\033[%d;%df", p1PosY, 2);
    puts("|");
    printf("\033[%d;%df", p1PosY + 1, 2);
    puts("|");
    printf("\033[%d;%df", p1PosY + 2, 2);
    puts("|");
}

void drawP2() {
    //draw the second paddle
    printf("\033[%d;%df", p2PosY - 2, WIDTH - 2);
    puts("|");
    printf("\033[%d;%df", p2PosY - 1, WIDTH - 2);
    puts("|");
    printf("\033[%d;%df", p2PosY, WIDTH - 2);
    puts("|");
    printf("\033[%d;%df", p2PosY + 1, WIDTH - 2);
    puts("|");
    printf("\033[%d;%df", p2PosY + 2, WIDTH - 2);
    puts("|");
}

void drawBall() {
    //draw ball in the middle of the screen
    printf("\033[%d;%df", HEIGHT / 2, WIDTH / 2);
    puts("o");
}

void drawScore() {
    //draw score
    printf("\033[%d;%df", 1, WIDTH / 2 - 4);
    printf("  %02d : %02d  ", p1Score, p2Score);
}

void init() {
    //set console dimensions to 80x24
    printf("\033[8;24;80t");
    //draw screen
    drawScreen();
    printf("q - quit | w - p1 up | s - p1 down | o - p2 up | l - p2 down");
    //set paddles in proper positions
    p1PosY = HEIGHT / 2;
    p2PosY = HEIGHT / 2;
    //set ball in the middle of the screen
    ballPosX = WIDTH / 2;
    ballPosY = HEIGHT / 2;
    //draw paddles, ball, and score
    drawP1();
    drawP2();
    drawBall();
    drawScore();
    //disable input echo
    system("stty -echo");
    //directly consume input without user pressing enter
    system("stty -icanon");
    //hide cursor
    printf("\033[?25l");
}

void moveP1() {
    if (p1KeyPressed == 'w' && p1PosY-3 > 1) {
        //move paddle 1 up
        p1PosY--;
        printf("\033[%d;%df", p1PosY - 2, 2);
        puts("|");
        printf("\033[%d;%df", p1PosY + 3, 2);
        puts(" ");
    } else if (p1KeyPressed == 's' && p1PosY+3 < HEIGHT) {
        //move paddle 1 down
        p1PosY++;
        printf("\033[%d;%df", p1PosY + 2, 2);
        puts("|");
        printf("\033[%d;%df", p1PosY - 3, 2);
        puts(" ");
    }
}

void moveP2() {
    if (p2KeyPressed == 'o' && p2PosY-3 > 1) {
        //move paddle 2 up
        p2PosY--;
        printf("\033[%d;%df", p2PosY - 2, WIDTH - 2);
        puts("|");
        printf("\033[%d;%df", p2PosY + 3, WIDTH - 2);
        puts(" ");
    } else if (p2KeyPressed == 'l' && p2PosY+3 < HEIGHT) {
        //move paddle 2 down
        p2PosY++;
        printf("\033[%d;%df", p2PosY + 2, WIDTH - 2);
        puts("|");
        printf("\033[%d;%df", p2PosY - 3, WIDTH - 2);
        puts(" ");
        }
}

void *userControl() {
    while (1) {
        //get user input
        int input = getchar();

        //if user pressed q, quit. if user pressed w or s, move paddle 1. if user pressed o or l, move paddle 2
        if (input == 'q') {
            quit();
        } else if (input == 'w' || input == 's') {
            p1KeyPressed = input;
            moveP1();
        } else if (input == 'o' || input == 'l') {
            p2KeyPressed = input;
            moveP2();
        }
    }
}

void moveBall() {
    //clear last ball
    printf("\033[%d;%df", ballPosY, ballPosX);
    puts(" ");

    if (ballDirection == 0) {
        //move ball up and left
        if (ballPosY-1 > 1 && ballPosX-1 > 1) {
            ballPosY--;
            ballPosX--;
        }
    } else if (ballDirection == 1) {
        //move ball up and right
        if (ballPosY-1 > 1 && ballPosX+1 < WIDTH-1) {
            ballPosY--;
            ballPosX++;
        }
    } else if (ballDirection == 2) {
        //move ball down and left
        if (ballPosY+1 < HEIGHT-1 && ballPosX-1 > 1) {
            ballPosY++;
            ballPosX--;
        }
    } else if (ballDirection == 3) {
        //move ball down and right
        if (ballPosY+1 < HEIGHT-1 && ballPosX+1 < WIDTH-1) {
            ballPosY++;
            ballPosX++;
        }
    }

    //check if ball hit paddle 1
    if (ballPosX == 3 && ballPosY >= p1PosY-2 && ballPosY <= p1PosY+2) {
        //move ball right
        if (ballDirection == 0) {
            ballDirection = 1;
        } else if (ballDirection == 2) {
            ballDirection = 3;
        }
    }
    //check if ball hit paddle 2
    if (ballPosX == WIDTH-4 && ballPosY >= p2PosY-2 && ballPosY <= p2PosY+2) {
        //move ball left
        if (ballDirection == 1) {
            ballDirection = 0;
        } else if (ballDirection == 3) {
            ballDirection = 2;
        }
    }
    //check if ball hit top or bottom wall
    if (ballPosY == 2) {
        if (ballDirection == 0) {
            ballDirection = 2;
        } else if (ballDirection == 1) {
            ballDirection = 3;
        }
    } else if (ballPosY == HEIGHT-2) {
        if (ballDirection == 2) {
            ballDirection = 0;
        } else if (ballDirection == 3) {
            ballDirection = 1;
        }
    }

    //check if ball hit left or right wall and add score
    if (ballPosX == 2) {
        //move ball to the middle of the screen
        ballPosX = WIDTH / 2;
        ballPosY = HEIGHT / 2;

        //add score to player 2
        p2Score++;
        drawScore();
    } if (ballPosX == WIDTH-2) {
        //move ball to the middle of the screen
        ballPosX = WIDTH / 2;
        ballPosY = HEIGHT / 2;

        //add score to player 1
        p1Score++;
        drawScore();
    }

    //draw ball
    printf("\033[%d;%df", ballPosY, ballPosX);
    puts("o");
}


int main() {
    //initialize game
    init();

    //create thread to let users control paddles
    pthread_t thread;
    pthread_create(&thread, NULL, userControl, NULL);


    //randomize ball direction
    srand(time(NULL));
    ballDirection = rand() % 4;

    while (1) {
        moveBall();
        sleep_ms(BALLSPEED);
    }

    quit();
    return 0;
}

//gcc pong.c -lpthread -o pong && ./pong