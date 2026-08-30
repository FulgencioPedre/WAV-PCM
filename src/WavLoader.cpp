#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <miniaudio.h>

#include "AutoCorrelation.h"
#include "WavLoader.h"

using namespace std;




bool WavLoader::LoadWav(const string& audio, SoundData& sound){
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