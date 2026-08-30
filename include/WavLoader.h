#pragma once

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class WavLoader{
    public:
        bool LoadWav(const string& audio, SoundData& sound);
};