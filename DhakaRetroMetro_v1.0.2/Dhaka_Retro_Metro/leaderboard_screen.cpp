#define _CRT_SECURE_NO_WARNINGS
#include "leaderboard_screen.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <vector>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
void iLine(double x1, double y1, double x2, double y2);
unsigned int iLoadImage(char filename[]);

static unsigned int sBoardBg = 0;
static unsigned int sBackButton = 0;

static int sScreenW = 0;
static int sScreenH = 0;

static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 60;
static int sBackH = 60;

struct LeaderboardEntry {
    char name[32];
    double totalTime;
};

static std::vector<LeaderboardEntry> sEntries;
static bool sLeaderboardLoaded = false;
static int sAnimFrame = 0; // For entry animations
static int sHoverClear = 0; // For button hover effect
static int sNewRecordIndex = -1; // Index of the newly added record for animation
static double sLastPlayerTime = -1.0; // Most recent session time
static char sLastPlayerName[32] = ""; // Most recent session name

// Dust particles for "cool" theme
static const int sDustCount = 40;
static float sDustX[sDustCount];
static float sDustY[sDustCount];
static float sDustVX[sDustCount];
static float sDustVY[sDustCount];
static float sDustR[sDustCount];

void sortLeaderboard() {
    std::sort(sEntries.begin(), sEntries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.totalTime > b.totalTime;
    });
    if (sEntries.size() > 10) {
        sEntries.resize(10);
    }
}

void loadLeaderboard() {
    sEntries.clear();
    FILE* f = fopen("leaderboard.txt", "r");
    if (!f) return;

    LeaderboardEntry temp;
    while (fscanf(f, "%31[^|]|%lf\n", temp.name, &temp.totalTime) == 2) {
        sEntries.push_back(temp);
    }
    fclose(f);
    sortLeaderboard();
    sLeaderboardLoaded = true;
}

void saveLeaderboard() {
    FILE* f = fopen("leaderboard.txt", "w");
    if (!f) return;

    for (const auto& entry : sEntries) {
        fprintf(f, "%s|%.3f\n", entry.name, entry.totalTime);
    }
    fclose(f);
}

void clearLeaderboard() {
    sEntries.clear();
    saveLeaderboard();
}

void addToLeaderboard(const char* name, double totalTime) {
    if (!sLeaderboardLoaded) loadLeaderboard();
    
    // Store for "Your Rank" display even if not in top 10
    strncpy(sLastPlayerName, name, 31);
    sLastPlayerName[31] = '\0';
    sLastPlayerTime = totalTime;

    // Duplicate prevention: if player exists, only update if time is better
    int foundIdx = -1;
    for (int i = 0; i < (int)sEntries.size(); i++) {
        if (strcmp(sEntries[i].name, name) == 0) {
            foundIdx = i;
            break;
        }
    }

    if (foundIdx != -1) {
        if (totalTime > sEntries[foundIdx].totalTime) {
            sEntries[foundIdx].totalTime = totalTime;
        }
    } else {
        LeaderboardEntry newEntry;
        const char* finalName = name;
        if (strlen(name) == 0) finalName = "Unknown Ninja";
        
        strncpy(newEntry.name, finalName, 31);
        newEntry.name[31] = '\0';
        newEntry.totalTime = totalTime;
        sEntries.push_back(newEntry);
    }

    sortLeaderboard();
    saveLeaderboard();
    
    // Find new position for animation highlight
    sNewRecordIndex = -1;
    for (int i = 0; i < (int)sEntries.size(); i++) {
        if (strcmp(sEntries[i].name, name) == 0 && sEntries[i].totalTime == totalTime) {
            sNewRecordIndex = i;
            break;
        }
    }

    sAnimFrame = 0; // Trigger slide-in animation
}

void initLeaderboardScreen(int screenW, int screenH) {
    sScreenW = screenW;
    sScreenH = screenH;

    sBoardBg = iLoadImage((char *)"Images/02.3 Leaderboard/bg.png");
    sBackButton = iLoadImage((char *)"Images/02.3 Leaderboard/back_button.png");

    sBackX = 30;
    sBackY = sScreenH - 90;
    sBackW = 60;
    sBackH = 60;

    srand((unsigned int)time(0));
    for (int i = 0; i < sDustCount; i++) {
        sDustX[i] = (float)(rand() % screenW);
        sDustY[i] = (float)(rand() % screenH);
        sDustVX[i] = -0.2f - (float)(rand() % 40) / 100.0f;
        sDustVY[i] = -0.05f - (float)(rand() % 30) / 100.0f;
        sDustR[i] = 1.0f + (float)(rand() % 20) / 10.0f;
    }
    
    loadLeaderboard();
}

void drawLeaderboardScreen(int screenW, int screenH) {
    iClear();
    if (sBoardBg) iShowImage(0, 0, screenW, screenH, sBoardBg);

    // Particle background
    for (int i = 0; i < sDustCount; i++) {
        iSetColor(255, 150, 60);
        iFilledCircle(sDustX[i], sDustY[i], sDustR[i]);
        sDustX[i] += sDustVX[i];
        sDustY[i] += sDustVY[i];
        if (sDustX[i] < -20.0f || sDustY[i] < -20.0f) {
            sDustX[i] = (float)(screenW + (rand() % 200));
            sDustY[i] = (float)(rand() % screenH);
        }
    }

    // Leaderboard UI Box
    int boxW = 800;
    int boxH = 500;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2 - 20;

    // Outer Frame
    iSetColor(10, 10, 20);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    iSetColor(100, 200, 255);
    iRectangle(boxX, boxY, boxW, boxH);
    iRectangle(boxX + 2, boxY + 2, boxW - 4, boxH - 4);

    // Header Color
    iText(boxX + 50, boxY + boxH - 50, (char*)"RANK", (void*)2);
    iText(boxX + 220, boxY + boxH - 50, (char*)"NINJA NAME", (void*)2);
    iText(boxX + boxW - 250, boxY + boxH - 50, (char*)"TIME", (void*)2);

    iLine(boxX + 30, boxY + boxH - 65, boxX + boxW - 30, boxY + boxH - 65);

    // Empty State Check
    if (sEntries.empty()) {
        iSetColor(150, 150, 150);
        iText(boxX + boxW/2 - 80, boxY + boxH/2, (char*)"No Records Yet", (void*)2);
    }

    // Draw Entries
    for (int i = 0; i < (int)sEntries.size(); i++) {
        // Slide-in animation: offset depends on sAnimFrame and index
        float animOffset = 200.0f - (sAnimFrame * 10.0f) + (i * 20.0f);
        if (animOffset < 0) animOffset = 0;

        int entryY = boxY + boxH - 110 - (i * 45);
        int entryX = boxX + (int)animOffset;
        
        // Ranking Highlights
        if (i == 0) {
            iSetColor(255, 215, 0); // Gold
            iFilledCircle(entryX + 50, entryY + 5, 12);
            iSetColor(0, 0, 0);
            iText(entryX + 46, entryY - 2, (char*)"1", (void*)2);
            iSetColor(255, 215, 0);
        } else if (i == 1) {
            iSetColor(192, 192, 192); // Silver
            iFilledCircle(entryX + 50, entryY + 5, 12);
            iSetColor(0, 0, 0);
            iText(entryX + 46, entryY - 2, (char*)"2", (void*)2);
            iSetColor(192, 192, 192);
        } else if (i == 2) {
            iSetColor(205, 127, 50);  // Bronze
            iFilledCircle(entryX + 50, entryY + 5, 12);
            iSetColor(0, 0, 0);
            iText(entryX + 46, entryY - 2, (char*)"3", (void*)2);
            iSetColor(205, 127, 50);
        } else {
            iSetColor(200, 200, 200);
            char rankStr[8];
            sprintf(rankStr, "%d", i + 1);
            iText(entryX + 46, entryY, rankStr, (void*)2);
        }

        // Highlight newest record with a pulse effect
        if (i == sNewRecordIndex) {
            double pulse = (sin(sAnimFrame * 0.2) + 1.0) / 2.0;
            iSetColor(100 + (int)(pulse * 155), 200 + (int)(pulse * 55), 255);
            iText(entryX + 350, entryY, (char*)"NEW!", (void*)2);
        }

        iText(entryX + 220, entryY, sEntries[i].name, (void*)2);

        char timeStr[32];
        int mins = (int)sEntries[i].totalTime / 60;
        int secs = (int)sEntries[i].totalTime % 60;
        int ms = (int)((sEntries[i].totalTime - (int)sEntries[i].totalTime) * 100); // 2 decimal precision
        sprintf(timeStr, "%02d:%02d.%02d", mins, secs, ms);
        iText(entryX + boxW - 180, entryY, timeStr, (void*)2);
    }

    // DRAW YOUR RANK (if not in top 10)
    if (sLastPlayerTime > 0) {
        bool inTop10 = false;
        for (int i = 0; i < (int)sEntries.size(); i++) {
            if (strcmp(sEntries[i].name, sLastPlayerName) == 0) { inTop10 = true; break; }
        }

        if (!inTop10) {
            iSetColor(100, 100, 150);
            iFilledRectangle(boxX + 30, boxY + 15, boxW - 60, 40);
            iSetColor(255, 255, 255);
            char yourRankStr[64];
            int mins = (int)sLastPlayerTime / 60;
            int secs = (int)sLastPlayerTime % 60;
            int ms = (int)((sLastPlayerTime - (int)sLastPlayerTime) * 100); // 2 decimal precision
            sprintf(yourRankStr, "YOU: %s (NOT IN TOP 10) - %02d:%02d.%02d", sLastPlayerName, mins, secs, ms);
            iText(boxX + 50, boxY + 28, yourRankStr, (void*)2);
        }
    }

    // Clear Button - Top Right of the box (Header)
    int clearW = 160, clearH = 35;
    int clearX = boxX + boxW - clearW - 20;
    int clearY = boxY + boxH - 55;
    if (sHoverClear) iSetColor(255, 100, 100);
    else iSetColor(150, 50, 50);
    iFilledRectangle(clearX, clearY, clearW, clearH);
    iSetColor(255, 255, 255);
    iRectangle(clearX, clearY, clearW, clearH);
    iText(clearX + 5, clearY + 12, (char*)"CLEAR LEADERBOARD", (void*)2);

    if (sAnimFrame < 100) sAnimFrame++;

    iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
}

bool handleLeaderboardClick(int mx, int my) {
    if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY && my <= sBackY + sBackH) {
        return true;
    }
    // Clear Button Check (Header top-right)
    int boxW = 800;
    int boxH = 500;
    int boxX = (sScreenW - boxW) / 2;
    int boxY = (sScreenH - boxH) / 2 - 20;
    int clearW = 160, clearH = 35;
    int clearX = boxX + boxW - clearW - 20;
    int clearY = boxY + boxH - 55;
    if (mx >= clearX && mx <= clearX + clearW && my >= clearY && my <= clearY + clearH) {
        clearLeaderboard();
        sLastPlayerTime = -1.0;
    }
    return false;
}

int getPlayerRank(double totalTime) {
    if (!sLeaderboardLoaded) loadLeaderboard();
    for (int i = 0; i < (int)sEntries.size(); i++) {
        if (totalTime >= sEntries[i].totalTime) {
            return i + 1;
        }
    }
    if (sEntries.size() < 10) return (int)sEntries.size() + 1;
    return -1; // Not in top 10
}

void setLeaderboardHover(int mx, int my) {
    int boxW = 800;
    int boxH = 500;
    int boxX = (sScreenW - boxW) / 2;
    int boxY = (sScreenH - boxH) / 2 - 20;
    int clearW = 160, clearH = 35;
    int clearX = boxX + boxW - clearW - 20;
    int clearY = boxY + boxH - 55;
    if (mx >= clearX && mx <= clearX + clearW && my >= clearY && my <= clearY + clearH) {
        sHoverClear = 1;
    } else {
        sHoverClear = 0;
    }
}
