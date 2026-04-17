#define _CRT_SECURE_NO_WARNINGS
#include "game_over_screen.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iLine(double x1, double y1, double x2, double y2);

static int sScreenW = 0;
static int sScreenH = 0;
static int sHoverIdx = -1; // 0: Retry, 1: Leaderboard, 2: Menu

// Button Layout
static int sBtnW = 200;
static int sBtnH = 50;
static int sBtnX = 0;
static int sBtnY[3] = {0};

void initGameOverScreen(int screenW, int screenH) {
    sScreenW = screenW;
    sScreenH = screenH;
    sBtnX = (screenW - sBtnW) / 2;
    int startY = 220;
    for (int i = 0; i < 3; i++) {
        sBtnY[i] = startY - i * (sBtnH + 20);
    }
}

void drawGameOverScreen(int screenW, int screenH, double finalTime, int timer) {
    // 1. Dark Overlay (Dithered/Scanline feel)
    iSetColor(0, 0, 0);
    for (int i = 0; i < screenH; i += 3) {
        iFilledRectangle(0, i, screenW, 2);
    }

    // 2. Main Modal Box (Glassmorphism inspired)
    int boxW = 500;
    int boxH = 450;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2;

    iSetColor(20, 10, 10);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    iSetColor(255, 50, 50); // Aggressive Red Border
    iRectangle(boxX, boxY, boxW, boxH);
    iRectangle(boxX + 2, boxY + 2, boxW - 4, boxH - 4);

    // 3. Title - EXACTLY "GameOver" as requested - Centered
    iSetColor(255, 50, 50);
    iText(boxX + 210, boxY + boxH - 60, (char*)"GameOver", (void*)2);
    
    // Subtle glow effect for title (Bottom underline)
    iSetColor(255, 50, 50);
    iLine(boxX + 200, boxY + boxH - 65, boxX + 300, boxY + boxH - 65);


    // 4. Score Display
    char timeStr[64];
    int mins = (int)finalTime / 60;
    int secs = (int)finalTime % 60;
    int ms = (int)((finalTime - (int)finalTime) * 100);
    sprintf(timeStr, "TOTAL NINJA TIME: %02d:%02d.%02d", mins, secs, ms);
    iSetColor(255, 255, 255);
    iText(boxX + 110, boxY + boxH - 120, timeStr, (void*)2);

    // 5. Buttons - Only show after the 3-second delay
    if (timer <= 0) {
        char* btnTexts[] = {(char*)"RETRY LEVEL", (char*)"METRO LEGENDS", (char*)"MAIN MENU"};
        for (int i = 0; i < 3; i++) {
            if (sHoverIdx == i) {
                iSetColor(255, 80, 80);
                iFilledRectangle(sBtnX - 5, sBtnY[i] - 5, sBtnW + 10, sBtnH + 10);
                iSetColor(255, 255, 255);
            } else {
                iSetColor(100, 20, 20);
                iFilledRectangle(sBtnX, sBtnY[i], sBtnW, sBtnH);
                iSetColor(200, 50, 50);
            }
            iRectangle(sBtnX, sBtnY[i], sBtnW, sBtnH);
            iSetColor(255, 255, 255);
            
            // Text positioning inside button
            int textX = sBtnX + (sBtnW - strlen(btnTexts[i]) * 9) / 2;
            iText(textX, sBtnY[i] + 18, btnTexts[i], (void*)2);
        }
    } else {
        // Show "Wait..." or just the title during delay
        iSetColor(150, 150, 150);
        char waitMsg[32];
        sprintf(waitMsg, "Analyzing results... %ds", (timer / 60) + 1);
        iText(boxX + 150, boxY + 100, waitMsg, (void*)2);
    }
}

GameOverAction handleGameOverClick(int mx, int my) {
    for (int i = 0; i < 3; i++) {
        if (mx >= sBtnX && mx <= sBtnX + sBtnW && my >= sBtnY[i] && my <= sBtnY[i] + sBtnH) {
            if (i == 0) return GO_RETRY;
            if (i == 1) return GO_LEADERBOARD;
            if (i == 2) return GO_MENU;
        }
    }
    return GO_NONE;
}

void setGameOverHover(int mx, int my) {
    sHoverIdx = -1;
    for (int i = 0; i < 3; i++) {
        if (mx >= sBtnX && mx <= sBtnX + sBtnW && my >= sBtnY[i] && my <= sBtnY[i] + sBtnH) {
            sHoverIdx = i;
            return;
        }
    }
}
