#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>

using namespace std;

#pragma pack(push, 1) //Evita que haya un padding entre medias de cada dato y byte
//Representa los datos que hay dentro del HEADER de un archivo WAV
struct WAVHeader{
    char riff[4];
    uint32_t overall_size;
    char wave[4];
};

struct WAVAdditional{
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t sample_alignment;
    uint16_t bits_per_sample;
};

struct WAVChunk{
    char id[4];
    uint32_t size;
};
#pragma pack(pop) //Elimina el requisito de evitar padding


//Datos que necesitamos para analizar el archivo WAV
struct SoundData{
    uint32_t sample_rate = 0;
    uint16_t channels = 0;
    vector<float> samples;
};


bool LoadWav(const string& audio, SoundData& sound){
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


void AutoCorrelation(const SoundData &sound){
    const size_t windowSize = 2400;
    ofstream correlation("correlations.txt");

    
    size_t startOffset = (sound.sample_rate*2) * 1;
    if(startOffset + windowSize > sound.samples.size()) return;

    float r;
    float minCorrelation = 1e9f;
    size_t bestLag = 0;
    size_t minLag = sound.sample_rate / 500;
    size_t maxLag = sound.sample_rate / 80;

   

    for(size_t i = minLag; i <= maxLag; ++i){
        
        r = 0.0f;
        correlation << "LAG: " << i << " -> ";
        for(size_t j = 0; j < windowSize - i; ++j){
            float sample1 =  sound.samples[startOffset + j];
            float sample2 = sound.samples[startOffset + j + i];
            float delta = sample1 - sample2;
            r += delta * delta;
        }
        correlation << i << ": "<< r << '\n';

        if(r < minCorrelation){
            minCorrelation = r;
            bestLag = i;
        }
    }
    float frequency = sound.sample_rate/bestLag;
    float MIDI = 69 + (12 * (log2(frequency/440)));
    correlation << "BEST LAG: " << bestLag << '\n';
    correlation << "FREQUENCY: " << frequency << '\n';
    correlation << "MIDI NOTE: " << MIDI << '\n';
    correlation.close(); 


}


int main(int argc, char* argv[]){
    if(argc <= 1){
        cout << "No WAV file inserted in programme" << endl;
        return 1;
    }

    SoundData sound;
    bool isValid = LoadWav(argv[1], sound);
    if(!isValid){
        cout << "Something went wrong!" << endl;
        return 1;
    }

    cout << "Sample Rates: " << sound.sample_rate << endl;
    cout << "Channels: " << sound.channels << endl;
    for(size_t i = 96000; (i < 96300 && i < sound.samples.size()); i++){
        cout << sound.samples[i] << endl;
    }

    AutoCorrelation(sound);

    return 0;      
}