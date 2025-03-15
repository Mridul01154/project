#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 50
#define COLS 150
#define DROP_RATE 70
#define COLOR_ROTATION_INTERVAL 10  // Change colors every 10 frames

#define COLOR_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define COLOR_RED (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLOR_BLUE (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COLOR_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COLOR_MAGENTA (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY)

bool running = true;
int grid[ROWS][COLS] = {0};
int colorGrid[ROWS][COLS] = {0};  

HANDLE hConsole;
SMALL_RECT windowSize = {0, 0, COLS - 1, ROWS - 1};
CHAR_INFO buffer[ROWS * COLS];

WORD sandColors[] = {COLOR_YELLOW, COLOR_RED, COLOR_BLUE, COLOR_WHITE, COLOR_MAGENTA};
int numColors = sizeof(sandColors) / sizeof(sandColors[0]);
int colorIndex = 0;  
int frameCount = 10; 

void hideCursor() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void renderBuffer() {
    COORD bufferSize = {COLS, ROWS};
    COORD bufferCoord = {0, 0}; 
    SMALL_RECT writeRegion = {5, 5, COLS, ROWS}; 

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;
            if (grid[i][j] == 1) {  
                buffer[index].Attributes = colorGrid[i][j];  
                if (COLS%2==0)
                buffer[index].Char.UnicodeChar = 0x2580; 

                else
                buffer[index].Char.UnicodeChar = 0x2584;  

            } else {
                buffer[index].Attributes = 0;  
                buffer[index].Char.AsciiChar = ' '; 
            }
        }
    }

    WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);
}

DWORD WINAPI fall(LPVOID lpParam) {
    while (running) {
        bool moved = false;

        
        if (++frameCount >= COLOR_ROTATION_INTERVAL) {
            frameCount = 0;
            colorIndex = (colorIndex + 1) % numColors; 
        }
        
        for (int i = ROWS - 2; i >= 0; i--) {
            for (int j = 0; j < COLS; j++) {
                if (grid[i][j] == 1) {
                    
                    if (grid[i + 1][j] == 0) {
                        grid[i + 1][j] = 1;
                        grid[i][j] = 0;
                        colorGrid[i + 1][j] = colorGrid[i][j]; 
                        moved = true;
                    }
                    
                    else if (grid[i + 1][j] == 1) {
                        bool leftEmpty = (j > 0 && grid[i + 1][j - 1] == 0);
                        bool rightEmpty = (j < COLS - 1 && grid[i + 1][j + 1] == 0);

                        if (leftEmpty && rightEmpty) {
                            if (rand() % 2 == 0) {
                                grid[i + 1][j - 1] = 1;
                                colorGrid[i + 1][j - 1] = sandColors[colorIndex]; 
                            } else {
                                grid[i + 1][j + 1] = 1;
                                colorGrid[i + 1][j + 1] = sandColors[colorIndex]; 
                            }
                            grid[i][j] = 0;
                            moved = true;
                        } 
                        else if (leftEmpty) {
                            grid[i + 1][j - 1] = 1;
                            colorGrid[i + 1][j - 1] = sandColors[colorIndex]; 
                            grid[i][j] = 0;
                            moved = true;
                        } 
                        else if (rightEmpty) {
                            grid[i + 1][j + 1] = 1;
                            colorGrid[i + 1][j + 1] = sandColors[colorIndex]; 
                            grid[i][j] = 0;
                            moved = true;
                        }
                    }
                }
            }
        }

        if (moved) {
            renderBuffer();
            Sleep(50);
        } else {
            Sleep(10);
        }
    }
    return 0;
}

DWORD WINAPI handleMouseClick(LPVOID lpParam) {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD numRead;
    INPUT_RECORD inputRecord;

    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    while (running) {
        ReadConsoleInput(hInput, &inputRecord, 1, &numRead);
        
        if (inputRecord.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mer = inputRecord.Event.MouseEvent;
            if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                int x = mer.dwMousePosition.X;  
                int y = mer.dwMousePosition.Y;

                if (x > 0 && x < COLS - 1 && y > 0 && y < ROWS - 1) {
                    
                    WORD currentColor = sandColors[colorIndex];

                    if (rand() % 100 < DROP_RATE) {
                        grid[y][x] = 1;
                        colorGrid[y][x] = currentColor;  
                    }
                    if (rand() % 100 < DROP_RATE) {
                        grid[y + 1][x] = 1;
                        colorGrid[y + 1][x] = currentColor;
                    }
                    if (rand() % 100 < DROP_RATE) {
                        grid[y - 1][x] = 1;
                        colorGrid[y - 1][x] = currentColor;
                    }
                    if (rand() % 100 < DROP_RATE) {
                        grid[y][x + 1] = 1;
                        colorGrid[y][x + 1] = currentColor;
                    }
                    if (rand() % 100 < DROP_RATE) {
                        grid[y][x - 1] = 1;
                        colorGrid[y][x - 1] = currentColor;
                    }

                    renderBuffer();
                }
            }
        } else if (inputRecord.EventType == KEY_EVENT) {
            if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
                running = false;
                break;
            }
        }
    }
    return 0;
}

int main() {
    srand(time(NULL));
    hideCursor();
    renderBuffer();
    
    HANDLE hMouseThread = CreateThread(NULL, 0, handleMouseClick, NULL, 0, NULL);
    HANDLE hFallThread = CreateThread(NULL, 0, fall, NULL, 0, NULL);
    
    WaitForSingleObject(hMouseThread, INFINITE);
    WaitForSingleObject(hFallThread, INFINITE);
    
    CloseHandle(hMouseThread);
    CloseHandle(hFallThread);
    
    return 0;
}
