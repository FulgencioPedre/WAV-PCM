#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <miniaudio.h>

#include "Pitch.h"

using namespace std;

const size_t WINDOW_SIZE = 2400;


bool Pitch::LoadWav(const string& audio, SoundData& sound){
    //Lee el archivo binario y lo guarda en un buffer, siendo este el header que creamos como variable
    ifstream file(audio, ios::binary);
    if(!file.is_open()){
        cout << "Error opening WAV file" << endl;
        return false;
    }  

    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(string(header.riff, 4) != "RIFF" || string(header.wave, 4) != "WAVE"){
        cout << "File is not a validate WAV file" << endl;
        return false;
    }  

    

    WAVChunk fmtChunk;
    file.read(reinterpret_cast<char*>(&fmtChunk), sizeof(fmtChunk));

    if(string(fmtChunk.id, 4) != "fmt "){
        cout << "fmt chunk not found" << endl;
        return false;
    }

    if(fmtChunk.size != sizeof(WAVAdditional)){
        cout << "Only standard PCM WAV files are supported" << endl;
        return false;
    }

    WAVAdditional additional;
    file.read(reinterpret_cast<char*>(&additional), fmtChunk.size);

    if(additional.channels != 1){
        cout << "Only mono WAV files are supported" << endl;
        return false;
    }
    if(additional.format_type != 1){
        cout << "Only PCM WAV files are supported" << endl;
        return false;
    }

    
    sound.sample_rate = additional.sample_rate;
    sound.channels = additional.channels;

    WAVChunk chunk;
    bool foundData = false;
    
    while(file.read(reinterpret_cast<char*>(&chunk), sizeof(chunk))){
        if(string(chunk.id, 4) == "data"){
            foundData = true;
            break;
        }

        file.seekg(chunk.size, ios::cur);
    }
    cout << "Found chunk: " << string(chunk.id, 4) << endl;
    cout << "Chunk size: " << chunk.size << endl;
    cout << "Position: " << file.tellg() << endl;

    if(!foundData){
        cout << "Data chunk not found"  << endl;
        return false;
    }

    cout << chunk.size << endl;
    if(additional.bits_per_sample == 16){

        //Al ser audio de 16 bits. si dividimos entre 2, nos da el numero de muestras de sonido que tiene la cancion
        size_t numSamples = chunk.size / sizeof(int16_t);
        vector<int16_t> rawBuffer(numSamples);
        file.read(reinterpret_cast<char*>(rawBuffer.data()), chunk.size);

        //Aqui normalizamos cada muestra de sonido, ya que al ser de 16 bits, uede llegar hasta -32768/+32767
        sound.samples.resize(numSamples);
        for(size_t i = 0; i < numSamples; ++i){
            sound.samples[i] = rawBuffer[i] / 32768.0f;
        }
    }

    ofstream output("samples.txt");
     for(size_t i = 96000; (i < 96300 && i < sound.samples.size()); i++){
        output << sound.samples[i] << '\n';
    }
    output.close();
    return true;
}


void Pitch::AutoCorrelation(const SoundData &sound, size_t offset){
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

