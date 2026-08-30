#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <miniaudio.h>

#include "AutoCorrelation.h"

using namespace std;






float Pitch::AutoCorrelation(const vector<float>& samples, uint32_t sampleRate){
    if(samples.size() > WINDOW_SIZE) return 0.0f;
    size_t windowSize = WINDOW_SIZE;

    float r;
    size_t minLag = sampleRate / 500;
    size_t maxLag = sampleRate / 80;
    float minCorrelation = 1e9f;
    size_t bestLag = 0;

    float sum = 0.0f;
    for(size_t j = 0; j < WINDOW_SIZE; ++j){
        sum += samples[j]*samples[j];
    }

    float rms = sqrt(sum/WINDOW_SIZE);
    if(rms < 0.01f){
        return 0.0f;
    }
    else{
        

        for(size_t i = minLag; i <= maxLag; ++i){
            r = 0.0f;

            for(size_t j = 0; j < WINDOW_SIZE - i; ++j){
                float sample1 =  samples[j];
                float sample2 = samples[j + i];
                float delta = sample1 - sample2;
                r += delta * delta;
            }
            

            if(r < minCorrelation){
                minCorrelation = r;
                bestLag = i;
            }
        }
        
    }
    if(bestLag > 0){
            float frequency = static_cast<float>(sampleRate)/bestLag;
            float midiNote = 69 + (12 * (log2(frequency/440)));
            return midiNote;
    }
    return 0.0f;
}

void Pitch::AutoCorrelationFiles(const SoundData &sound, size_t offset){
    ofstream correlation("correlations.txt");

    size_t windowSize = WINDOW_SIZE;
    size_t startOffset = 0;
    if(startOffset + windowSize > sound.samples.size()) return;

    float r;
    size_t minLag = sound.sample_rate / 500;
    size_t maxLag = sound.sample_rate / 80;

   while(true){
    if(startOffset + windowSize > sound.samples.size()){
        break;
    }
    float sum = 0.0f;
    for(size_t j = 0; j < WINDOW_SIZE; ++j){
        float sample =  sound.samples[startOffset + j];
        sum += sample*sample;
    }

    float timestamp = static_cast<float>(startOffset)/sound.sample_rate;
    float rms = sqrt(sum/WINDOW_SIZE);
    if(rms < 0.01f){
        correlation << timestamp << "\t0.0\t0.0\n";
        startOffset += offset;
    }
    else{
        float minCorrelation = 1e9f;
        size_t bestLag = 0;

        for(size_t i = minLag; i <= maxLag; ++i){
            r = 0.0f;
            //correlation << "LAG: " << i << " -> ";
            for(size_t j = 0; j < WINDOW_SIZE - i; ++j){
                float sample1 =  sound.samples[startOffset + j];
                float sample2 = sound.samples[startOffset + j + i];
                float delta = sample1 - sample2;
                r += delta * delta;
            }
            //correlation << i << ": "<< r << '\n';

            if(r < minCorrelation){
                minCorrelation = r;
                bestLag = i;
            }
        }
        startOffset += offset;
        float frequency = static_cast<float>(sound.sample_rate)/bestLag;
        float midiNote = 69 + (12 * (log2(frequency/440)));
        correlation << timestamp << '\t' << frequency << '\t' << midiNote << '\n';
    }
   }

    correlation.close(); 
}