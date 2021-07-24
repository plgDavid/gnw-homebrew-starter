#pragma once

#include <stdint.h>

#include "main.h"

// Default to 50Hz as it results in more samples than at 60Hz
#define AUDIO_SAMPLE_RATE   (48000)
#define AUDIO_BUFFER_LENGTH (AUDIO_SAMPLE_RATE / 50)

void gw_audio_start(void);
void gw_audio_stop(void);


//CLIENT process call: IMPLEMENT ME!!
//IMPORTANT NOTE, the SAI/Amp has a hard time dealing with the high current used when playing high RMS content!!!
//It reboots at different thresholds for different users. 
//Especially if the battery is unplugged and only USB power is applied.
//More investigation needed
void gw_audio_process(int16_t*buffer, uint32_t samples);