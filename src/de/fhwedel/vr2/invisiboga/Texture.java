package de.fhwedel.vr2.invisiboga;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

/**
 * Repräsentiert eine Textur (aus QCAR-Beispielen übernommen).<br>
 * 
 * @author QCAR
 *
 */
public class Texture {
    public int mWidth;
    public int mHeight;
    public int mChannels;
    public byte[] mData;

    public byte[] getData() {
        return mData;
    }

   /*
    * Lädt eine bestimmte Textur aus dem Android-Package.
    */
    public static Texture loadTextureFromApk(String fileName, AssetManager assets) {
        Log.i("Loading texture \"%s\" from APK", fileName);
        InputStream inputStream = null;
        try {
            inputStream = assets.open(fileName, AssetManager.ACCESS_BUFFER);
            BufferedInputStream bufferedStream = new BufferedInputStream(inputStream);
            Bitmap bitMap = BitmapFactory.decodeStream(bufferedStream);
            int[] data = new int[bitMap.getWidth() * bitMap.getHeight()];
            bitMap.getPixels(data, 0, bitMap.getWidth(), 0, 0, bitMap.getWidth(),
                bitMap.getHeight());
            byte[] dataBytes = new byte[bitMap.getWidth() * bitMap.getHeight() * 4];
            for (int p = 0; p < bitMap.getWidth() * bitMap.getHeight(); ++p) {
                int colour = data[p];
                dataBytes[p * 4] = (byte) (colour >>> 16);
                dataBytes[p * 4 + 1] = (byte) (colour >>> 8);
                dataBytes[p * 4 + 2] = (byte) colour;
                dataBytes[p * 4 + 3] = (byte) (colour >>> 24);
            }
            Texture texture = new Texture();
            texture.mWidth = bitMap.getWidth();
            texture.mHeight = bitMap.getHeight();
            texture.mChannels = 4;
            texture.mData = dataBytes;
            return texture;
        } catch (IOException e) {
            Log.e("Failed to load texture \"%s\" from APK.", fileName);
            Log.e(e.getMessage());
            return null;
        }
    }
}
