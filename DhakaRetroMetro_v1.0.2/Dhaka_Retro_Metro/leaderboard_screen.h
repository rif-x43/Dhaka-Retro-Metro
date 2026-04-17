#pragma once

void initLeaderboardScreen(int screenW, int screenH);
void drawLeaderboardScreen(int screenW, int screenH);
bool handleLeaderboardClick(int mx, int my);
void addToLeaderboard(const char* name, double totalTime);
int getPlayerRank(double totalTime);
void clearLeaderboard();
void setLeaderboardHover(int mx, int my);
