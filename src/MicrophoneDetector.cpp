#include "miniaudio.h"
#include "MicrophoneDetector.h"
#include "AutoCorrelation.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;



static vector<float> audioBuffer(2400, 0.0f);

void MicrophoneDetector::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
    const float* samples = (const float*)pInput;
    if(samples == NULL) return;
    

    if(frameCount < WINDOW_SIZE){
        for(size_t i = 0; i < WINDOW_SIZE - frameCount; i++){
            audioBuffer[i] = audioBuffer[i + frameCount];
        }
        for(size_t i = 0; i < frameCount; i++){
            audioBuffer[WINDOW_SIZE - frameCount + i] = samples[i];
        }
    }

    Pitch pitch;
    float currentMidi = pitch.AutoCorrelation(audioBuffer, pDevice->sampleRate);
    if(currentMidi > 0.0f){
        cout << "MIDI NOTE: " << currentMidi << "\r" << flush;
    }

}


int MicrophoneDetector::MicConfig(){
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32; //sets format of device recording
    config.capture.channels = 1; //1 for MONO, 2 for BI...
    config.sampleRate = 48000; //samples in a second (0 for default sample rate)
    config.dataCallback = data_callback;

    ma_device device;
    if(ma_device_init(NULL, &config, &device) != MA_SUCCESS) return -1; //failed to initialize device

    ma_device_start(&device); //device sleeping at default so need to start

    cout << "PROGRAM IS NOW ACTIVE, SING TO THE MIC! (PUSH ENTER IF WANT TO EXIT)" << endl;
    cin.get();
    
    ma_device_uninit(&device);
    return 0;

}