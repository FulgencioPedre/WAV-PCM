#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

#pragma pack(push, 1) //Evita que haya un padding entre medias de cada dato y byte
//Representa los datos que hay dentro del HEADER de un archivo WAV
struct WAVHeader{
    char riff[4];
    uint32_t overall_size;
    char wave[4];
    char fmt[4];
    uint32_t length_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t sample_alignment;
    uint16_t bits_per_sample;
    char data_header[4];
    uint32_t data_size;

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

    if(header.channels != 1){
        cout << "Only mono WAV files are supported" << endl;
        return false;
    }
    if(header.format_type != 1){
        cout << "Only PCM WAV files are supported" << endl;
        return false;
    }

    sound.sample_rate = header.sample_rate;
    sound.channels = header.channels;

    if(header.bits_per_sample == 16){

        //Al ser audio de 16 bits. si dividimos entre 2, nos da el numero de muestras de sonido que tiene la cancion
        size_t numSamples = header.data_size / sizeof(int16_t);
        vector<int16_t> rawBuffer(numSamples);
        file.read(reinterpret_cast<char*>(rawBuffer.data()), header.data_size);

        //Aqui normalizamos cada muestra de sonido, ya que al ser de 16 bits, uede llegar hasta -32768/+32768
        sound.samples.resize(numSamples);
        for(size_t i = 0; i < numSamples; ++i){
            sound.samples[i] = rawBuffer[i] / 32768.0f;
        }
    }

    return true;
}


int main(int argc, char* argv[]){

}