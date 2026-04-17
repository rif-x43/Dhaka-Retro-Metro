
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
unsigned int iLoadImage(char filename[]);

static bool sLevel4AssetsLoaded = false;

// Screen dimensions
static int sScreenW = 0;
static int sScreenH = 0;

// Backgrounds
static unsigned int sBg1Tex = 0;
static unsigned int sBgVictory = 0;

// Level Phases
enum LevelPhase { PHASE_CODE_GATE, PHASE_SEQ_TEXT, PHASE_SEQ_IMAGE, COMPLETED };
static LevelPhase sCurrentPhase = PHASE_CODE_GATE;

// Animation logic
static int sSeqTimer = 0;
static double sZoomScale = 0.1;

// Kakashi Win Sprites
static const int WIN_FRAME_COUNT = 11;
static unsigned int sKakashiWinRight[WIN_FRAME_COUNT];

// Animation state
static int sWinFrame = 0;
static int sWinTimer = 0;
static int sElapsedFrames = 0;

// Code Gate System
static int sCodeGateStep = 0; 
static char sCodeBuffer[32];
static int sCodeBufferPos = 0;
static bool sCodeError = false;
static int sCodeErrorTimer = 0;

static const char *sExpectedCodes[3] = {
  "1DC 75UA",  // Arena 1 code
  "1DC T5UA",  // Arena 2 code
  "1AC T5VB"   // Arena 3 code
};
static const char *sCodeLabels[3] = {
  "ARENA 1 SECRET CODE",
  "ARENA 2 SECRET CODE",
  "ARENA 3 SECRET CODE"
};
static bool sCodesVerified[3] = {false, false, false};

void initLevel4(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  if (sLevel4AssetsLoaded) return;

  sBg1Tex = iLoadImage((char *)"Images/Level 4 Background/1.png");
  sBgVictory = iLoadImage((char *)"Images/Arena/ARENA_4.png");

  // Load Kakashi Win frames
  for (int i = 0; i < WIN_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Win Right/WinRight%d.png", i + 1);
    sKakashiWinRight[i] = iLoadImage(path);
  }

  sLevel4AssetsLoaded = true;
}

void resetLevel4() {
  if (!sLevel4AssetsLoaded) {
    initLevel4(sScreenW > 0 ? sScreenW : 1080, sScreenH > 0 ? sScreenH : 635);
  }

  sCurrentPhase = PHASE_CODE_GATE;
  sElapsedFrames = 0;
  sSeqTimer = 0;
  sZoomScale = 0.1;
  
  sCodeGateStep = 0;
  memset(sCodeBuffer, 0, sizeof(sCodeBuffer));
  sCodeBufferPos = 0;
  sCodeError = false;
  sCodeErrorTimer = 0;
  sCodesVerified[0] = false;
  sCodesVerified[1] = false;
  sCodesVerified[2] = false;

  sWinFrame = 0;
  sWinTimer = 0;
}

void drawLevel4(int screenW, int screenH) {
  iClear();

  if (sCurrentPhase == PHASE_CODE_GATE) {
    // Background
    iShowImage(0, 0, screenW, screenH, sBg1Tex);

    // Overlay
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, screenW, screenH);

    // Header
    iSetColor(255, 100, 100);
    iText(screenW / 2.0 - 150, screenH - 60, (char *)"SECRET METRO GATE - FINAL STAGE", (void *)2);
    iSetColor(200, 200, 200);
    iText(screenW / 2.0 - 200, screenH - 90, (char *)"Enter the codes obtained from the previous three arenas");

    // Input fields
    for (int c = 0; c < 3; c++) {
      int boxX = screenW / 2 - 150;
      int boxY = screenH - 180 - (c * 100);

      // Label
      if (sCodesVerified[c]) iSetColor(0, 255, 0);
      else if (c == sCodeGateStep) iSetColor(255, 200, 50);
      else iSetColor(100, 100, 100);
      iText(boxX, boxY + 40, (char *)sCodeLabels[c], (void *)2);

      // Box
      iSetColor(20, 20, 30);
      iFilledRectangle(boxX, boxY, 300, 35);
      
      // Border
      if (c == sCodeGateStep) iSetColor(255, 255, 255);
      else iSetColor(50, 50, 80);
      iRectangle(boxX, boxY, 300, 35);

      // Text
      if (sCodesVerified[c]) {
        iSetColor(0, 255, 0);
        iText(boxX + 10, boxY + 10, (char *)"CODE VERIFIED", (void *)2);
      } else if (c == sCodeGateStep) {
        iSetColor(255, 255, 255);
        iText(boxX + 10, boxY + 10, sCodeBuffer, (void *)2);
        // Cursor
        if ((sElapsedFrames / 20) % 2 == 0) {
          iFilledRectangle(boxX + 10 + sCodeBufferPos * 12, boxY + 5, 2, 25);
        }
      } else {
        iSetColor(50, 50, 50);
        iText(boxX + 10, boxY + 10, (char *)"********", (void *)2);
      }
    }

    if (sCodeError) {
      iSetColor(255, 50, 50);
      iText(screenW / 2 - 80, 100, (char *)"INCORRECT CODE!", (void *)2);
    }
  } else if (sCurrentPhase == PHASE_SEQ_TEXT) {
      // Fullscreen Dark Overlay
      iSetColor(5, 5, 10);
      iFilledRectangle(0, 0, screenW, screenH);
      
      // Animated Sequential Text
      // Line 1: NEW RECORD! (Gold)
      if (sSeqTimer > 30) {
          double alpha = (sSeqTimer - 30) / 60.0;
          if (alpha > 1.0) alpha = 1.0;
          iSetColor(255 * alpha, 215 * alpha, 0); // Gold
          iText(screenW / 2 - 180, screenH / 2 + 80, (char *)"NEW RECORD! RANK #1 ON METRO LEGENDS", (void *)2);
      }
      
      // Line 2: CONGO! (Yellow)
      if (sSeqTimer > 120) {
          double alpha = (sSeqTimer - 120) / 60.0;
          if (alpha > 1.0) alpha = 1.0;
          iSetColor(255 * alpha, 255 * alpha, 0); // Bright Yellow
          iText(screenW / 2 - 170, screenH / 2 + 30, (char *)"CONGO! YOU HAVE FINISHED THE GAME!", (void *)2);
      }

      // Line 3: More updates... (Gray)
      if (sSeqTimer > 210) {
          double alpha = (sSeqTimer - 210) / 60.0;
          if (alpha > 1.0) alpha = 1.0;
          iSetColor(180 * alpha, 180 * alpha, 180 * alpha); // Light Gray
          iText(screenW / 2 - 150, screenH / 2 - 20, (char *)"More updates will be coming soon!", (void *)2);
      }
      
      // Retro glow simulated by tiny offset draw (only for finished text)
      if (sSeqTimer > 300) {
          iSetColor(100, 100, 0);
          iText(screenW / 2 - 181, screenH / 2 + 81, (char *)"NEW RECORD! RANK #1 ON METRO LEGENDS", (void *)2);
          iText(screenW / 2 - 171, screenH / 2 + 31, (char *)"CONGO! YOU HAVE FINISHED THE GAME!", (void *)2);
      }

  } else if (sCurrentPhase == PHASE_SEQ_IMAGE) {
      iSetColor(5, 5, 10);
      iFilledRectangle(0, 0, screenW, screenH);
      
      // Zoom-in effect for the Congo Image
      int imgW = (int)(screenW * sZoomScale);
      int imgH = (int)(screenH * sZoomScale);
      iShowImage(screenW / 2 - imgW / 2, screenH / 2 - imgH / 2, imgW, imgH, sBgVictory);

  } else {
    // COMPLETED Phase - Final State
    iShowImage(0, 0, screenW, screenH, sBgVictory);

    iSetColor(20, 20, 20);
    iFilledRectangle(0, screenH / 2 - 100, screenW, 200);
    
    iSetColor(255, 215, 0);
    iText(screenW / 2 - 250, screenH / 2 + 30, (char *)"CONGO! YOU HAVE FINISHED THE GAME!", (void *)2);
    iSetColor(200, 200, 255);
    iText(screenW / 2 - 180, screenH / 2 - 20, (char *)"More updates will be coming soon!", (void *)2);
    
    iSetColor(150, 150, 150);
    iText(screenW / 2 - 120, screenH / 2 - 70, (char *)"Wait for Leaderboard...", (void *)1);

    // Draw Kakashi Win animation
    iShowImage(screenW / 2 - 60, 50, 120, 120, sKakashiWinRight[sWinFrame]);
  }
}

void updateLevel4() {
  sElapsedFrames++;
  
  if (sCodeErrorTimer > 0) {
    sCodeErrorTimer--;
    if (sCodeErrorTimer <= 0) sCodeError = false;
  }

  if (sCurrentPhase == PHASE_SEQ_TEXT) {
      sSeqTimer++;
      if (sSeqTimer > 450) { // 7.5 seconds total for text sequence
          sCurrentPhase = PHASE_SEQ_IMAGE;
          sSeqTimer = 0;
      }
  } else if (sCurrentPhase == PHASE_SEQ_IMAGE) {
      sSeqTimer++;
      if (sZoomScale < 1.0) sZoomScale += 0.015; // Smooth zoom in
      if (sZoomScale > 1.0) sZoomScale = 1.0;
      
      if (sSeqTimer > 180) { // 3 seconds zoom and stay
          sCurrentPhase = COMPLETED;
      }
  } else if (sCurrentPhase == COMPLETED) {
    sWinTimer++;
    if (sWinTimer >= 8) {
      sWinTimer = 0;
      sWinFrame = (sWinFrame + 1) % WIN_FRAME_COUNT;
    }
  }
}

void level4Keyboard(unsigned char key) {
  if (sCurrentPhase == PHASE_CODE_GATE) {
    if (key == 13) { // Enter
      if (_stricmp(sCodeBuffer, sExpectedCodes[sCodeGateStep]) == 0) {
        sCodesVerified[sCodeGateStep] = true;
        sCodeGateStep++;
        memset(sCodeBuffer, 0, sizeof(sCodeBuffer));
        sCodeBufferPos = 0;
        sCodeError = false;
        
        if (sCodeGateStep > 2) {
          sCurrentPhase = PHASE_SEQ_TEXT;
          sSeqTimer = 0;
        }
      } else {
        sCodeError = true;
        sCodeErrorTimer = 120;
      }
    } else if (key == 8) { // Backspace
      if (sCodeBufferPos > 0) {
        sCodeBufferPos--;
        sCodeBuffer[sCodeBufferPos] = '\0';
      }
    } else if (sCodeBufferPos < 30) {
        if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') ||
            (key >= '0' && key <= '9') || key == ' ') {
          char c = key;
          if (c >= 'a' && c <= 'z') c -= 32;
          sCodeBuffer[sCodeBufferPos] = c;
          sCodeBufferPos++;
          sCodeBuffer[sCodeBufferPos] = '\0';
          sCodeError = false;
        }
    }
  }
}

void level4SpecialKeyboard(unsigned char key) {}
void level4KeyboardUp(unsigned char key) {}
void level4SpecialKeyboardUp(unsigned char key) {}

bool isLevel4GameOver() { return false; }
bool isLevel4GameWon() { return sCurrentPhase == COMPLETED; }
