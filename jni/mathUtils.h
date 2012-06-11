/**
 * @file mathUtils.h
 *
 * @brief Schnittstelle zu mathUtils.cpp.
 *
 * @author QCAR
 *
 * @date 17.06.2011
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <QCAR/Tool.h>

QCAR::Vec2F vec2FAdd(QCAR::Vec2F, QCAR::Vec2F);
QCAR::Vec2F vec2FSub(QCAR::Vec2F v1, QCAR::Vec2F v2);
QCAR::Vec3F vec3FAdd(QCAR::Vec3F v1, QCAR::Vec3F v2);
QCAR::Vec3F vec3FSub(QCAR::Vec3F v1, QCAR::Vec3F v2);
QCAR::Vec2F vec2FScale(QCAR::Vec2F, float);
QCAR::Vec3F vec3FScale(QCAR::Vec3F v, float s);
QCAR::Vec4F vec3FDiv(QCAR::Vec4F v1, float s);
float vec3FDot(QCAR::Vec3F v1, QCAR::Vec3F v2);
QCAR::Vec3F vec3FNormalize(QCAR::Vec3F v);
QCAR::Vec4F vec4FTransform(QCAR::Vec4F& v, QCAR::Matrix44F& m);
QCAR::Matrix44F matrix44FIdentity();
float matrix44FDeterminate(QCAR::Matrix44F& m);
QCAR::Matrix44F matrix44FInverse(QCAR::Matrix44F& m);
float vec2FDistance(QCAR::Vec2F v1, QCAR::Vec2F v2);
QCAR::Vec2F normalize(QCAR::Vec2F v);
bool linePlaneIntersection(QCAR::Vec3F lineStart, QCAR::Vec3F lineEnd, QCAR::Vec3F pointOnPlane,
        QCAR::Vec3F planeNormal, QCAR::Vec3F &intersection);
void projectScreenPointToPlane(float screenWidth, float screenHeight, QCAR::Vec2F point,
        QCAR::Matrix44F inverseProjectionMatrix, QCAR::Matrix44F modelViewMatrix,
        QCAR::Vec3F planeCenter, QCAR::Vec3F planeNormal, QCAR::Vec3F &intersection);
void translatePoseMatrix(float x, float y, float z, float* nMatrix = NULL);
void scalePoseMatrix(float x, float y, float z, float* nMatrix = NULL);
void rotatePoseMatrix(float angle, float x, float y, float z, float* nMatrix = NULL);

#endif
