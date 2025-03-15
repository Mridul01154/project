#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define HEIGHT 30
#define WIDTH 60


struct Node {
    int x, y;
    struct Node* next;
};

struct Snake {
    struct Node* head;
    char direction;
};

int food_x, food_y, score = 0;

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void draw_boundary() {
    for (int i = 0; i < WIDTH; i++) {
        gotoxy(i, 0); printf("#");
        gotoxy(i, HEIGHT - 1); printf("#");
    }
    for (int i = 0; i < HEIGHT; i++) {
        gotoxy(0, i); printf("#");
        gotoxy(WIDTH - 1, i); printf("#");
    }
}

void spawn_food() {
    food_x = (rand() % (WIDTH - 2)) + 1;
    food_y = (rand() % (HEIGHT - 2)) + 1;
    gotoxy(food_x, food_y);
    printf("*");
}

void add_segment(struct Snake* snake) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->next = snake->head;
    new_node->x = snake->head->x;
    new_node->y = snake->head->y;
    snake->head = new_node;
}

void move_snake(struct Snake* snake) {
    int prev_x = snake->head->x;
    int prev_y = snake->head->y;
    int new_x = prev_x, new_y = prev_y;

    if (snake->direction == 'd') new_x++;
    else if (snake->direction == 'a') new_x--;
    else if (snake->direction == 's') new_y++;
    else if (snake->direction == 'w') new_y--;

    struct Node* temp = snake->head;
    while (temp->next != NULL) {
        int temp_x = temp->next->x;
        int temp_y = temp->next->y;
        temp->next->x = prev_x;
        temp->next->y = prev_y;
        prev_x = temp_x;
        prev_y = temp_y;
        temp = temp->next;
    }
    snake->head->x = new_x;
    snake->head->y = new_y;
}

int check_collision(struct Snake* snake) {
    if (snake->head->x <= 0 || snake->head->x >= WIDTH - 1 ||
        snake->head->y <= 0 || snake->head->y >= HEIGHT - 1) return 1;

    struct Node* temp = snake->head->next;
    while (temp != NULL) {
        if (snake->head->x == temp->x && snake->head->y == temp->y) return 1;
        temp = temp->next;
    }
    return 0;
}

void display(struct Snake* snake) {
    struct Node* temp = snake->head;
    while (temp != NULL) {
        gotoxy(temp->x, temp->y);
        printf("O");
        temp = temp->next;
    }
    gotoxy(snake->head->x, snake->head->y);
    printf("@");
}

void clear_tail(struct Snake* snake) {
    struct Node* temp = snake->head;
    while (temp->next != NULL) temp = temp->next;
    gotoxy(temp->x, temp->y);
    printf(" ");
}

int main() {
    struct Snake snake;
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    head->x = WIDTH / 2;
    head->y = HEIGHT / 2;
    head->next = NULL;
    snake.head = head;
    snake.direction = 'd';

    system("cls");
    draw_boundary();
    spawn_food();

    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if ((key == 'w' && snake.direction != 's') ||
                (key == 's' && snake.direction != 'w') ||
                (key == 'a' && snake.direction != 'd') ||
                (key == 'd' && snake.direction != 'a')) {
                snake.direction = key;
            }
        }

        clear_tail(&snake);
        move_snake(&snake);

        if (check_collision(&snake)) break;

        if (snake.head->x == food_x && snake.head->y == food_y) {
            score++;
            add_segment(&snake);
            spawn_food();
        }

        display(&snake);
        Sleep(100);
    }

    system("cls");
    printf("GAME OVER! Your Score: %d\n", score);
    Sleep(2000);
    return 0;
}
