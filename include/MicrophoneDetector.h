#pragma once

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class MicrophoneDetector{
    public:
        int MicConfig();
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 framCount);
};