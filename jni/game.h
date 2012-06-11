/**
 * @file game.h
 *
 * @brief Schnittstelle zu game.cpp.
 *
 * @author Alexander Bertram
 *
 * @date 23.06.2011
 */

#ifndef GAME_H
#define GAME_H

#include "types.h"

void initGame();
void restartGame();
int getGameState();
void updateGameState(int);
void processMove(float);

#endif
