/**
 * @file Texture.h
 *
 * @brief Schnittstelle zu Texture.cpp (aus QCAR-Beispielen übernommen).
 *
 * @author Alexander Bertram
 *
 * @date 28.05.2011
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <jni.h>

class Texture {
public:
    Texture();
    ~Texture();
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    static Texture* create(JNIEnv* env, jobject textureObject);
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mChannelCount;
    unsigned char* mData;
    unsigned int mTextureID;
};

#endif
