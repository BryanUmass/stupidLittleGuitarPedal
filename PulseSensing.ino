#include <limits>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/MaximilianDSP.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "maximilian.h"
#include <FS.h>
#include <SD_MMC.h>


// Define Arduino output
AudioBoardStream out(NoBoard);
Maximilian maximilian(out);

// MAXIMILIAN
maxiSample beats; //We give our sample a name. It's called beats this time. We could have loads of them, but they have to have different names.

void setup() {//some inits
    // setup logging
    Serial.begin(115200);
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

    // setup audio output
    auto cfg = out.defaultConfig(TX_MODE);
    out.begin(cfg);
    maximilian.begin(cfg);

    // setup SD to allow file operations
    if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

    //load in your samples: beat2.wav is too big - but snare.wav will work
    //beats.load("/sdcard/Maximilian/beat2.wav");
    //beats.load("/sdcard/Maximilian/snare.wav");
    beats.load("/13 - Plastic Beach.wav");
    //get info on samples if you like.
    Serial.println(beats.getSummary());

}

//this is where the magic happens. Very slow magic.
void play(float *output) {
    output[0]=beats.play();//just play the file. Looping is default for all play functions.
    //output[0]=beats.playAtSpeed(0.68);//play the file with a speed setting. 1. is normal speed.
    //output[0]=beats.playAtSpeedBetweenPoints(0.5,0,13300);//linear interpolationplay with a frequency input, start point and end point. Useful for syncing.
    //output[0]=beats.play4(0.5,0,13300);//cubic interpolation play with a frequency input, start point and end point. Useful for syncing.
    
    output[1]=output[0];
}

// Arduino loop
void loop() {
    maximilian.copy();
}








// #include <SD.h>
// #include <SPI.h>
// #include <SPIFFS.h>
// #include <ESP8266Audio.h>

// // SD card pin definitions
// #define SD_CS_PIN 5 // Change to your SD card CS pin

// float FIRCoeffs[] = {
// 0.000000000000000019,
// 0.060530312237274071,
// -0.000000000000000019,
// -0.100883853728790121,
// 0.000000000000000019,
// 0.302651561186370377,
// 0.475403960610291498,
// 0.302651561186370377,
// 0.000000000000000019,
// -0.100883853728790121,
// -0.000000000000000019,
// 0.060530312237274071,
// 0.000000000000000019
// };
// uint FIRlen = sizeof(FIRCoeffs)/sizeof(FIRCoeffs[0]);

// // applied using convolution
// void applyFIRFilter(float* input, float* output, int inputLength, const float* firCoeffs, int firLength) {
//     for (int i = 0; i < inputLength - firLength; i++) {
//         float sum = 0.0f;
//         for (int j = 0; j < firLength; j++) {
//             sum += input[i + j] * firCoeffs[j];
//         }
//         output[i] = sum;
//     }
// }

// float cubicInterpolate(float y0, float y1, float y2, float y3, float t) {
//     float a0, a1, a2, a3;

//     a0 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;
//     a1 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
//     a2 = -0.5f * y0 + 0.5f * y2;
//     a3 = y1;

//     return a0 * t * t * t + a1 * t * t + a2 * t + a3;
// }

// // Custom resampler class
// class AudioFileSourceResample : public AudioFileSource {
// private:
//     AudioFileSource *source;   // Underlying audio source
//     float pitchShift;          // Pitch adjustment factor
//     float readPointer;         // Current position for resampling
//     float inputBuffer[512];  // Buffer for raw audio data
//     float outputBuffer[512]; // Resampled audio data

//     uint32_t total_bytes_read = 0; // used to keep track of number of bytes read

// public:
//     AudioFileSourceResample(AudioFileSource *src, float pitch)
//         : source(src), pitchShift(pitch), readPointer(0.0) {}

//     bool open(const char *filename) override {
//         return source->open(filename);
//     }

//     uint32_t read(void *data, uint32_t len) override {
//         // Read raw audio from the source
//         size_t bytesRead = source->read(inputBuffer, len);
//         if (bytesRead == 0) return 0; // End of file

//         // DO NOT INTERPOLATE HEADER INFORMATION!!!
//         total_bytes_read += bytesRead;
//         //Serial.println(bytes_read);
//         if(total_bytes_read < 44) {
//           memcpy(data,inputBuffer,bytesRead);
//           return bytesRead;
//         }

//         // Apply lowpass filter
//         float firBuffer[512];
//         //applyFIRFilter(inputBuffer,firBuffer,len,FIRCoeffs,FIRlen);
//         memcpy(firBuffer,inputBuffer,512);

//         // Resample Audio (cooler version)
//         size_t newByteCount = max(1,int(len/pitchShift));
//         for(int i = 0; i < newByteCount; i++) {
//           float pos = i * pitchShift;
//           uint32_t index = uint32_t(pos);
//           float frac = pos - index;

//           // Fetch neighboring samples
//           int16_t sample1 = ((int16_t *)inputBuffer)[index];
//           int16_t sample2 = ((int16_t *)inputBuffer)[index + 1];

//           //int x0 = (int)index - 1;
//           //int x1 = (int)index;
//           //int x2 = (int)index + 1;
//           //int x3 = (int)index + 2;

//           // Clamp indices to valid range
//           //x0 = max(0, x0);
//           //x1 = max(0, x1);
//           //x2 = min(int(len - 1), x2);
//           //x3 = min(int(len - 1), x3);
//           //float mu = 0.5; //midway

//           // Interpolate
//           outputBuffer[i] = sample1;
//           //outputBuffer[i] = (1 - frac) * sample1 + frac * sample2;
//           //outputBuffer[i] = cubicInterpolate(firBuffer[x0], firBuffer[x1], firBuffer[x2], firBuffer[x3], mu);
//         }

//         // Copy resampled data to output buffer
//         memcpy(data, outputBuffer, newByteCount);
//         return newByteCount;
//     }

//     bool close() override {
//         return source->close();
//     }

//     bool isOpen() override {
//         return source->isOpen();
//     }
// };

// // Audio setup
// AudioGeneratorWAV *wav;
// AudioFileSourceSD *file;
// AudioFileSourceResample *resampler; // Custom resampler wrapper
// AudioOutputI2S *out;

// void setup() {
//     Serial.begin(115200);
//     SPI.begin();

//     // Initialize SD card
//     if (!SD.begin(SD_CS_PIN)) {
//         Serial.println("SD Card initialization failed!");
//         while (1);
//     }
//     Serial.println("SD Card initialized.");

//     // Initialize audio output
//     out = new AudioOutputI2S(0, 1, 26);
//     out->SetRate(48000);
//     out->begin();

//     // Set up audio chain
//     file = new AudioFileSourceSD("/13 - Plastic Beach.wav");
//     resampler = new AudioFileSourceResample(file, 2.0);
//     wav = new AudioGeneratorWAV();

//     if (!wav->begin(resampler, out)) {
//         Serial.println("Failed to start WAV playback");
//         while (1);
//     }
// }

// void loop() {
//     if (wav->isRunning()) {
//         if (!wav->loop()) {
//             wav->stop();
//             Serial.println("Playback finished");
//         }
//     } else {
//         delay(100); // Idle while waiting
//     }
// }




// // // #include <SD.h>
// // // #include <SPI.h>
// // // #include <SPIFFS.h>
// // // #include <ESP8266Audio.h>

// // // // SD card pin definitions
// // // #define SD_CS_PIN 5 // Change to your SD card CS pin


// // // // Audio setup
// // // AudioGeneratorWAV *wav;
// // // AudioFileSourceSD *file;
// // // AudioOutputI2S *out;

// // // void setup() {
// // //   Serial.begin(115200);
// // //   SPI.begin();
  
// // //   // Initialize SD card
// // //   if (!SD.begin(SD_CS_PIN)) {
// // //     Serial.println("SD Card initialization failed!");
// // //     while (1);
// // //   }
// // //   Serial.println("SD Card initialized.");

// // //   // Initialize audio
// // //   out = new AudioOutputI2S(0,1,26);
// // //   out->SetRate(480); // RAISE SAMPLE RATE TO CHANGE SPEED/PITCH
// // //   out->begin();
  
// // //   file = new AudioFileSourceSD("/13 - Plastic Beach.wav"); // Replace with your WAV file path
// // //   wav = new AudioGeneratorWAV();
// // //   if (!wav->begin(file, out)) {
// // //     Serial.println("Failed to start WAV playback");
// // //     while (1);
// // //   }
// // // }

// // // void loop() {
// // //   if (wav->isRunning()) {
// // //     if (!wav->loop()) {
// // //       wav->stop();
// // //       Serial.println("Playback finished");
// // //     }
// // //   } else {
// // //     delay(100); // Idle while waiting
// // //   }
// // // }
