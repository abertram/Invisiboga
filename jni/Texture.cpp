/**
 * @file Texture.cpp
 *
 * @brief Beinhaltet die Textur-Klasse (aus den QCAR-Beispielen übernommen).
 *
 * @author QCAR
 *
 * @date 28.05.2011
 */

#include <string.h>

#include "Texture.h"
#include "utils.h"

/**
 * @class Texture
 *
 * @brief Textur.
 */
Texture::Texture() :
        mWidth(0), mHeight(0), mChannelCount(0), mData(0), mTextureID(0) {
}

Texture::~Texture() {
    if (mData != 0) {
        delete[] mData;
    }
}

/**
 * Erezugt eine Textur-Instanz für die Verwendung auf der nativen Ebene aus einem Textur-Objekt auf
 * Java-Ebene.
 *
 * @param env JNI interface pointer.
 * @param textureObject Textur-Objekt.
 * @return Erzeugte Textur-Instanz.
 */
Texture* Texture::create(JNIEnv* env, jobject textureObject) {
    Texture* newTexture = new Texture();
    jclass textureClass = env->GetObjectClass(textureObject);
    jfieldID widthID = env->GetFieldID(textureClass, "mWidth", "I");
    if (!widthID) {
        LOG("Field mWidth not found.");
        return 0;
    }
    newTexture->mWidth = env->GetIntField(textureObject, widthID);
    jfieldID heightID = env->GetFieldID(textureClass, "mHeight", "I");
    if (!heightID) {
        LOG("Field mHeight not found.");
        return 0;
    }
    newTexture->mHeight = env->GetIntField(textureObject, heightID);
    newTexture->mChannelCount = 4;
    jmethodID texBufferMethodId = env->GetMethodID(textureClass, "getData", "()[B");
    if (!texBufferMethodId) {
        LOG("Function GetTextureBuffer() not found.");
        return 0;
    }
    jbyteArray pixelBuffer = (jbyteArray)env->CallObjectMethod(textureObject, texBufferMethodId);
    if (pixelBuffer == NULL) {
        LOG("Get image buffer returned zero pointer");
        return 0;
    }
    jboolean isCopy;
    jbyte* pixels = env->GetByteArrayElements(pixelBuffer, &isCopy);
    if (pixels == NULL) {
        LOG("Failed to get texture buffer.");
        return 0;
    }
    newTexture->mData = new unsigned char[newTexture->mWidth * newTexture->mHeight
            * newTexture->mChannelCount];unsigned
    char* textureData = (unsigned char*)pixels;
    int rowSize = newTexture->mWidth * newTexture->mChannelCount;
    for (int r = 0; r < newTexture->mHeight; ++r) {
        memcpy(newTexture->mData + rowSize * r, pixels + rowSize * (newTexture->mHeight - 1 - r),
                newTexture->mWidth * 4);
    }
    env->ReleaseByteArrayElements(pixelBuffer, pixels, 0);
    return newTexture;
}
