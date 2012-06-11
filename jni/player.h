/**
 * @file player.h
 *
 * @brief Schnittstelle zu player.cpp.
 *
 * @author Alexander Bertram
 *
 * @date 23.06.2011
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

void initPlayer(Player*);
int drawBeginningPlayer();
void setPlayerTargetSpace(Player*, int);
void preparePawnMove(Player*);
void prepareMove(Player*);
void processPlayerMove(Player*, float);

#endif
