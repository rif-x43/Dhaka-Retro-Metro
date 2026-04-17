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
static int sBtnW = 240;
static int sBtnH = 50;
static int sBtnX = 0;
static int sBtnY[3] = {0};
static int sBoxW = 520;
static int sBoxH = 480;

void initGameOverScreen(int screenW, int screenH) {
    sScreenW = screenW;
    sScreenH = screenH;
}

void drawGameOverScreen(int screenW, int screenH, double finalTime, int timer) {
    // 1. Dark Overlay (Dithered/Scanline feel)
    iSetColor(0, 0, 0);
    for (int i = 0; i < screenH; i += 3) {
        iFilledRectangle(0, i, screenW, 2);
    }

    // 2. Main Modal Box (Glassmorphism inspired)
    int boxX = (screenW - sBoxW) / 2;
    int boxY = (screenH - sBoxH) / 2;

    iSetColor(15, 10, 10);
    iFilledRectangle(boxX, boxY, sBoxW, sBoxH);
    iSetColor(255, 50, 50); // Aggressive Red Border
    iRectangle(boxX, boxY, sBoxW, sBoxH);
    iRectangle(boxX + 2, boxY + 2, sBoxW - 4, sBoxH - 4);

    // 3. Title - EXACTLY "GameOver" as requested - Centered
    iSetColor(255, 50, 50);
    iText(boxX + sBoxW / 2 - 45, boxY + sBoxH - 70, (char*)"GameOver", (void*)2);
    
    // Subtle glow effect for title (Bottom underline)
    iSetColor(255, 50, 50);
    iLine(boxX + sBoxW / 2 - 60, boxY + sBoxH - 75, boxX + sBoxW / 2 + 60, boxY + sBoxH - 75);

    // 4. Score Display
    char timeStr[64];
    int mins = (int)finalTime / 60;
    int secs = (int)finalTime % 60;
    int ms = (int)((finalTime - (int)finalTime) * 100);
    sprintf(timeStr, "TOTAL NINJA TIME: %02d:%02d.%02d", mins, secs, ms);
    iSetColor(255, 255, 255);
    iText(boxX + 80, boxY + sBoxH - 140, timeStr, (void*)2);

    // 5. Update Button Positions Dynamically for Strictly Internal Alignment
    sBtnX = boxX + (sBoxW - sBtnW) / 2;
    int btnGap = 20;
    int totalBtnsH = 3 * sBtnH + 2 * btnGap;
    int startY = boxY + (sBoxH - totalBtnsH) / 2 - 40; // Shifted slightly down from center

    for (int i = 0; i < 3; i++) {
        sBtnY[i] = startY + (2 - i) * (sBtnH + btnGap);
    }

    // 6. Draw Buttons - Only show after the 3-second delay
    if (timer <= 0) {
        char* btnTexts[] = {(char*)"RETRY LEVEL", (char*)"METRO LEGENDS", (char*)"MAIN MENU"};
        for (int i = 0; i < 3; i++) {
            if (sHoverIdx == i) {
                iSetColor(255, 80, 80);
                iFilledRectangle(sBtnX - 5, sBtnY[i] - 5, sBtnW + 10, sBtnH + 10);
                iSetColor(255, 255, 255);
            } else {
                iSetColor(30, 10, 10);
                iFilledRectangle(sBtnX, sBtnY[i], sBtnW, sBtnH);
                iSetColor(200, 50, 50);
            }
            iRectangle(sBtnX, sBtnY[i], sBtnW, sBtnH);
            iSetColor(255, 255, 255);
            
            // Text positioning inside button - Dynamically centered
            int textW = strlen(btnTexts[i]) * 9;
            int textX = sBtnX + (sBtnW - textW) / 2;
            iText(textX, sBtnY[i] + 18, btnTexts[i], (void*)2);
        }
    } else {
        // Show "Wait..." during delay
        iSetColor(150, 150, 150);
        char waitMsg[32];
        sprintf(waitMsg, "Analyzing results... %ds", (timer / 60) + 1);
        iText(boxX + sBoxW / 2 - 90, boxY + 100, waitMsg, (void*)2);
    }
}

GameOverAction handleGameOverClick(int mx, int my) {
    if (sBtnY[0] == 0) return GO_NONE; // Not yet drawn/initialized

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
    if (sBtnY[0] == 0) return;

    sHoverIdx = -1;
    for (int i = 0; i < 3; i++) {
        if (mx >= sBtnX && mx <= sBtnX + sBtnW && my >= sBtnY[i] && my <= sBtnY[i] + sBtnH) {
            sHoverIdx = i;
            return;
        }
    }
}
