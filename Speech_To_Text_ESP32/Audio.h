#ifndef _AUDIO_H
#define _AUDIO_H

#include <Arduino.h>
#include "I2S.h"

class Audio {
    I2S* i2s;
    static const int headerSize = 44;
    static const int i2sBufferSize = 12000;
    char i2sBuffer[i2sBufferSize];
    void CreateWavHeader(byte* header, int waveDataSize);

    public:
        static const int wavDataSize = 90000;
        static const int dividedWavDataSize = i2sBufferSize/4;
        char** wavData;
        byte paddedHeader[headerSize + 4] = {0};
      
        Audio(MicType micType);
        ~Audio();
        void Record();
};

#endif
