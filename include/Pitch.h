#pragma once

#include <cstdint>
#include <string>
#include <vector>
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

class Pitch{
    public:
        bool LoadWav(const string& audio, SoundData& sound);
        void AutoCorrelation(const SoundData &sound, size_t offset);
};

