#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <miniaudio.h>

#include "Pitch.h"

using namespace std;

int main(int argc, char* argv[]){
    if(argc <= 1){
        cout << "No WAV file inserted in programme" << endl;
        return 1;
    }
    Pitch pitch;
    SoundData sound;
    bool isValid = pitch.LoadWav(argv[1], sound);
    if(!isValid){
        cout << "Something went wrong!" << endl;
        return 1;
    }

    cout << "Sample Rates: " << sound.sample_rate << endl;
    cout << "Channels: " << sound.channels << endl;
    for(size_t i = 96000; (i < 96300 && i < sound.samples.size()); i++){
        cout << sound.samples[i] << endl;
    }

    pitch.AutoCorrelation(sound, 500);

    return 0;      
}