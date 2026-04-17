#pragma once

enum GameOverAction {
    GO_NONE,
    GO_RETRY,
    GO_LEADERBOARD,
    GO_MENU
};

void initGameOverScreen(int screenW, int screenH);
void drawGameOverScreen(int screenW, int screenH, double finalTime, int timer);
GameOverAction handleGameOverClick(int mx, int my);
void setGameOverHover(int mx, int my);
