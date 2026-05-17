//
// Created by opus arc on 2026/3/20.
//

#ifndef MOTIF_MYDRWAV_H
#define MOTIF_MYDRWAV_H

#include <vector>
#include <iostream>
#include <string>


class MyDrWav {
public:
    static std::vector<float> loadWav(
        const std::string& title,
        uint32_t targetSampleRate,
        uint32_t targetChannels
    );
    
    static std::vector<float> downmixStereoToMono(const std::vector<float>& stereo);
};


#endif //MOTIF_MYDRWAV_H
