/**
 * @file field.h
 *
 * @brief Schnitstelle zu field.cpp.
 *
 * @author Alexander Bertram
 *
 * @date 03.09.2011
 */

#ifndef FIELD_H
#define FIELD_H

bool canCreateSpace(QCAR::Vec2F);
void createSpace(QCAR::Vec2F);
Space* selectSpace(QCAR::Vec2F);

#endif
