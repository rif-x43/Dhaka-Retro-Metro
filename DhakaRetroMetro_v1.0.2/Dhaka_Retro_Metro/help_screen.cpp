#include "help_screen.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iText(double x, double y, char *str, void *font = 0);
unsigned int iLoadImage(char filename[]);

static unsigned int sHelpBg = 0;
static int sScreenW = 0;
static int sScreenH = 0;

static int sFadeAlpha = 0; // 0 to 255 for fade in

// Back Button positioned at bottom center
static int sBackW = 220;
static int sBackH = 55;
static int sBackX = 0;
static int sBackY = 0;
static bool sHoverBack = false;

// do not see ------------------------------------
static const int sDustCount = 40;
static float sDustX[sDustCount];
static float sDustY[sDustCount];
static float sDustVX[sDustCount];
static float sDustVY[sDustCount];
static float sDustR[sDustCount];
// do not see ------------------------------------

void initHelpScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  // Re-use main menu background for consistency
  sHelpBg = iLoadImage((char *)"Images/02. Menu/bg.png");

  // do not see ------------------------------------
  srand((unsigned int)time(0));
  for (int i = 0; i < sDustCount; i++) {
    sDustX[i] = (float)(rand() % screenW);
    sDustY[i] = (float)(rand() % screenH);
    sDustVX[i] = -0.2f - (float)(rand() % 40) / 100.0f;
    sDustVY[i] = -0.05f - (float)(rand() % 30) / 100.0f;
    sDustR[i] = 1.0f + (float)(rand() % 20) / 10.0f;
  }
  // do not see ------------------------------------
}

void resetHelpScreen() {
    sFadeAlpha = 0;
}

static void drawControlRow(int x, int y, int w, const char* label, const char* keys, double alpha) {
    iSetColor(240 * alpha, 240 * alpha, 255 * alpha);
    iText(x, y, (char*)label, (void*)2); // Standard pixel font

    // Dots for connecting
    iSetColor(60 * alpha, 60 * alpha, 80 * alpha);
    int dotCount = (int)((w - 350) / 12);
    for(int i=0; i<dotCount; i++) {
        iText(x + 220 + i*12, y, (char*)".", (void*)1);
    }

    iSetColor(100 * alpha, 215 * alpha, 255 * alpha); // Neon key highlight
    iText(x + w - 160, y, (char*)keys, (void*)2);
}

void drawHelpScreen(int screenW, int screenH) {
  iClear();
  if (sFadeAlpha < 255) sFadeAlpha += 8;
  double alphaFactor = (sFadeAlpha / 255.0);

  // 1. Background (Blurred menu feel)
  iSetColor(255, 255, 255);
  iShowImage(0, 0, screenW, screenH, sHelpBg);
  
  // Dark overlay
  iSetColor(0.0, 0.0, 5.0/255.0);
  // Simulate dimming
  for(int i=0; i<screenH; i+=2) {
      iSetColor(0.0, 0.0, 10.0/255.0);
      iFilledRectangle(0, i, screenW, 1);
  }

  // 2. Animated Dust (Ambient)
  for (int i = 0; i < sDustCount; i++) {
    iSetColor(100 * alphaFactor, 150 * alphaFactor, 255 * alphaFactor);
    iFilledCircle(sDustX[i], sDustY[i], sDustR[i]);
    sDustX[i] += sDustVX[i];
    sDustY[i] += sDustVY[i];
    if (sDustX[i] < -20.0f || sDustY[i] < -20.0f) {
      sDustX[i] = (float)(screenW + (rand() % 200));
      sDustY[i] = (float)(rand() % screenH);
    }
  }

  // 3. Main Controls Panel
  int pW = 900;
  int pH = 540;
  int pX = (screenW - pW) / 2;
  int pY = (screenH - pH) / 2 - 20;

  // Panel Background (Glossy)
  iSetColor(10, 10, 18);
  iFilledRectangle(pX, pY, pW, pH);
  
  // Neon Glow borders
  iSetColor(30, 100, 255); // Neon Blue
  iRectangle(pX, pY, pW, pH);
  iSetColor(255, 50, 80); // Neon Pink/Red corner accent
  iRectangle(pX - 2, pY - 2, pW + 4, pH + 4);

  // 4. TITLE
  iSetColor(255 * alphaFactor, 255 * alphaFactor, 255 * alphaFactor);
  iText(pX + pW/2 - 70, pY + pH - 45, (char*)"CONTROLS", (void*)2);
  
  iSetColor(255 * alphaFactor, 255 * alphaFactor, 255 * alphaFactor);
  // Manual underline
  double lineX[2] = {(double)pX + 50, (double)pX + pW - 50};
  double lineY = (double)pY + pH - 60;
  // iLine(lineX[0], lineY, lineX[1], lineY);
  iFilledRectangle(lineX[0], lineY, lineX[1]-lineX[0], 2);

  // 5. CONTENT SECTIONS
  int startY = pY + pH - 110;
  int colX[2] = { pX + 60, pX + pW/2 + 30 };
  int rowSpacing = 35;

  // --- SECTION 1: MOVEMENT ---
  iSetColor(255 * alphaFactor, 200 * alphaFactor, 50 * alphaFactor); // Gold Header
  iText(colX[0], startY, (char*)"RUNNING & JUMPING", (void*)2);
  drawControlRow(colX[0], startY - rowSpacing*1, 380, "Move Left", "A / Arrow L", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*2, 380, "Move Right", "D / Arrow R", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*3, 380, "Jump / Double", "W / Space / Up", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*4, 380, "Crouch / Drop", "S / Arrow D", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*5, 380, "Run Mode", "Hold L", alphaFactor);

  // --- SECTION 2: COMBAT ---
  iSetColor(255 * alphaFactor, 200 * alphaFactor, 50 * alphaFactor);
  iText(colX[0], startY - rowSpacing*7, (char*)"NINJA COMBAT (KAKASHI)", (void*)2);
  drawControlRow(colX[0], startY - rowSpacing*8, 380, "Knife Strike", "K", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*9, 380, "Throw Kunai", "J", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*10, 380, "Special Jutsu", "O", alphaFactor);
  drawControlRow(colX[0], startY - rowSpacing*11, 380, "Dialog Pager", "P", alphaFactor);

  // --- SECTION 3: SUDOKU ---
  iSetColor(100 * alphaFactor, 255 * alphaFactor, 100 * alphaFactor); // Green Header
  iText(colX[1], startY, (char*)"SUDOKU MINIGAME", (void*)2);
  drawControlRow(colX[1], startY - rowSpacing*1, 380, "Navigate", "Arrow Keys", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*2, 380, "Input Value", "1 - 9", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*3, 380, "Clear Cell", "0 / Backspace", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*4, 380, "Verify", "Enter", alphaFactor);

  // --- SECTION 4: GLOBAL & CHEATS ---
  iSetColor(255 * alphaFactor, 50 * alphaFactor, 50 * alphaFactor); // Red Header
  iText(colX[1], startY - rowSpacing*6, (char*)"GLOBAL CONTROL", (void*)2);
  drawControlRow(colX[1], startY - rowSpacing*7, 380, "Menu / Back", "ESC", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*8, 380, "Quick Menu", "M", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*9, 380, "Phase Skip", "0 (Zero)", alphaFactor);
  drawControlRow(colX[1], startY - rowSpacing*10, 380, "Instant Death", "I", alphaFactor);

  // --- 6. BACK BUTTON ---
  sBackW = 240;
  sBackH = 45;
  sBackX = pX + pW/2 - sBackW/2;
  sBackY = pY + 25;

  if (sHoverBack) {
      iSetColor(255, 50, 50);
      iFilledRectangle(sBackX - 4, sBackY - 4, sBackW + 8, sBackH + 8);
      iSetColor(255, 255, 255);
  } else {
      iSetColor(30, 30, 60);
      iFilledRectangle(sBackX, sBackY, sBackW, sBackH);
      iSetColor(255, 50, 50);
  }
  iRectangle(sBackX, sBackY, sBackW, sBackH);
  iText(sBackX + 85, sBackY + 15, (char*)"BACK", (void*)2);
}

void setHelpHover(int mx, int my) {
    if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY && my <= sBackY + sBackH) {
        sHoverBack = true;
    } else {
        sHoverBack = false;
    }
}

bool handleHelpClick(int mx, int my) {
  if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY && my <= sBackY + sBackH) {
    return true;
  }
  return false;
}
